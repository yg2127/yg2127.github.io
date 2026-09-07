---
layout: post
title: "PyTorch 03 - backward부터 detach까지, Autograd 흐름 읽기"
description: "zero_grad, backward, step이 나뉜 이유와 detach, clone, no_grad가 계산 그래프를 어떻게 다루는지 XOR 학습 코드로 정리"
date: 2026-09-07 09:20:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: pytorch, autograd, backward, detach, optimizer
comments: true
---

## 들어가며

XOR 모델을 만들고 나니 더 큰 의문이 생겼다.

> `loss.backward()`에는 모델을 전달하지 않는데, 어떻게 hidden layer와 output layer의 가중치를 전부 찾아가서 gradient를 계산하지?

처음에는 Python 코드가 위에서 아래로 실행되니 각 줄은 계산이 끝난 숫자만 다음 줄에 넘긴다고 생각했다. 하지만 PyTorch 텐서는 값만 저장하지 않는다. gradient가 필요한 연산에서는 **그 값이 어떤 연산을 거쳐 만들어졌는지**도 계산 그래프로 연결한다.

이 글은 XOR 학습 루프를 기준으로 `zero_grad → forward → loss → backward → step`을 따라가며 Autograd의 역할을 정리한 기록이다.

---

## 1. 학습 루프 전체부터 보기

```python
for epoch in range(1, 2001):
    model.train()

    optimizer.zero_grad()
    logits = model(X)
    loss = criterion(logits, y)
    loss.backward()
    optimizer.step()
```

다섯 줄의 역할은 명확히 분리되어 있다.

| 단계 | 코드 | 일어나는 일 |
|---:|---|---|
| 1 | `zero_grad()` | 이전 step에서 parameter의 `.grad`에 누적된 gradient 제거 |
| 2 | `model(X)` | 현재 parameter로 logits 계산, 계산 그래프 생성 |
| 3 | `criterion(logits, y)` | logits에서 scalar loss까지 그래프 연결 |
| 4 | `loss.backward()` | 그래프를 역방향으로 따라 parameter의 `.grad` 계산 |
| 5 | `optimizer.step()` | `.grad`를 읽어 parameter 값 갱신 |

`backward()`가 gradient를 계산하고, `step()`이 그 gradient를 사용해 실제 parameter를 바꾼다. 둘을 한 동작으로 생각하면 학습 코드가 자꾸 헷갈린다.

---

## 2. `loss.backward()`는 모델을 어떻게 아는가

forward 과정은 다음처럼 연결되어 있다.

$$
X \rightarrow \text{Linear}_1 \rightarrow \tanh
\rightarrow \text{Linear}_2 \rightarrow \text{logits}
\rightarrow \text{BCEWithLogitsLoss} \rightarrow \text{loss}
$$

`logits = model(X)`가 실행될 때, logits 텐서에는 output layer의 연산이 연결된다. 다시 output layer의 입력은 Tanh 결과와 연결되고, Tanh 결과는 hidden layer와 연결된다. 마지막에 만든 loss에서 출발해 이 연결을 거꾸로 따라가면 모든 parameter에 도달한다.

```python
loss.backward()
```

그래서 `backward()`에 모델을 다시 전달할 필요가 없다. loss가 이미 모델의 연산 그래프 끝에 있기 때문이다.

> `loss.backward()`는 “이 모델을 미분해”라는 명령이 아니라 **“이 loss가 만들어진 경로를 역추적해, 도달 가능한 leaf parameter들의 gradient를 계산해”**라는 명령에 가깝다.

---

## 3. gradient는 어디에 저장될까?

`backward()`가 parameter 값을 직접 바꾸지는 않는다. 각 parameter의 `.grad` 속성에 미분값을 저장한다.

```python
loss.backward()

print(model.hidden.weight.grad.shape)  # (4, 2)
print(model.output.weight.grad.shape)  # (1, 4)
```

gradient의 shape은 해당 parameter와 같다. optimizer는 생성될 때 받은 parameter들을 기억하고 있다.

```python
optimizer = torch.optim.Adam(model.parameters(), lr=0.05)
```

따라서 `optimizer.step()`은 등록된 각 parameter의 `.grad`를 읽어 Adam 규칙으로 값을 갱신할 수 있다.

---

## 4. 왜 매번 `zero_grad()`를 호출할까?

PyTorch는 `backward()`를 호출할 때 기존 `.grad`를 덮어쓰지 않고 더한다.

```python
optimizer.zero_grad()
loss.backward()
optimizer.step()
```

이 누적 방식은 여러 mini-batch의 gradient를 모으는 gradient accumulation에서 유용하다. 하지만 일반적인 학습 루프에서는 지난 step의 gradient가 섞이면 안 되므로 forward 전에 초기화한다.

`zero_grad(set_to_none=True)`를 쓰면 0으로 채운 텐서를 유지하는 대신 `.grad`를 `None`으로 돌려 메모리 작업을 줄일 수 있다. 다음 `backward()`에서 필요한 gradient가 새로 생성된다.

---

## 5. `detach()`는 무엇을 끊는가

학습 도중 parameter 값을 기록한다고 하자.

```python
snapshot = parameter.detach()
```

`detach()`는 현재 텐서와 같은 값을 바라보지만, 반환된 텐서를 기존 계산 그래프에서 분리한다. 이 텐서를 사용한 이후 연산은 원래 parameter 쪽으로 gradient를 보내지 않는다.

하지만 한 가지 함정이 있다. `detach()` 결과는 원본과 storage를 공유한다. optimizer가 원본 parameter를 갱신하면 “과거 값을 저장했다”고 생각한 snapshot도 함께 달라질 수 있다.

그래서 독립적인 과거 값을 보관하려면 `clone()`까지 필요하다.

```python
previous_params = [
    parameter.detach().clone()
    for parameter in model.parameters()
]
```

| 표현 | 그래프 연결 | 원본과 storage 공유 | 과거 값 보관에 적합 |
|---|---|---|---|
| `parameter` | O | 원본 | X |
| `parameter.detach()` | X | O | X |
| `parameter.clone()` | O | X | X |
| `parameter.detach().clone()` | X | X | O |

`clone()`을 먼저 하고 `detach()`해도 최종 결과는 그래프에서 분리된 독립 복사본이지만, `detach().clone()`이 “그래프를 끊고 값을 복사한다”는 의도를 읽기 쉽다.

---

## 6. `no_grad()`와 `detach()`의 차이

`detach()`는 특정 텐서 하나를 그래프에서 분리한다. `torch.no_grad()`는 context 안에서 실행되는 연산 전체에 대해 gradient 기록을 끈다.

```python
with torch.no_grad():
    squared_change = sum(
        ((parameter - previous) ** 2).sum()
        for parameter, previous in zip(
            model.parameters(), previous_params
        )
    )

    delta_history.append(torch.sqrt(squared_change).item())
```

가중치 변화량은 학습을 관찰하기 위한 통계일 뿐, 다음 loss를 만드는 데 쓰이지 않는다. 이 계산까지 그래프에 기록하면 메모리와 계산을 낭비한다.

`with`는 이 설정을 해당 블록에만 임시로 적용하는 Python context manager 문법이다. 블록을 빠져나오면 gradient 기록 설정은 원래대로 돌아온다.

---

## 7. gradient가 흐르는 경로와 막히는 경로

XOR 코드의 변수들을 기준으로 정리하면 다음과 같다.

| 대상 | loss에서 도달 가능? | 이유 |
|---|---|---|
| `output.weight`, `output.bias` | reachable | logits를 직접 만드는 parameter |
| `hidden.weight`, `hidden.bias` | reachable | Tanh와 output layer를 거쳐 loss에 연결 |
| `previous_params` | blocked by `detach` | 관찰용 복사본이며 그래프에서 분리 |
| `delta_history` 계산 | blocked by `no_grad` | parameter 변화량 기록용 |
| `loss.item()` 결과 | not involved | Python 숫자로 꺼내 기록만 함 |
| `prediction = probability >= 0.5` | not involved | 평가용 이산 예측, 학습 loss에 사용하지 않음 |

학습에 필요한 경로는 loss까지 끊기지 않고 이어져야 한다. 반대로 로그·그래프·평가를 위한 계산은 의도적으로 그래프 밖에서 수행한다.

---

## 8. 실제 변화량을 기록해보니

실습에서는 매 epoch 이후 현재 parameter와 직전 parameter 사이의 L2 거리를 계산했다.

$$
\|\theta_t-\theta_{t-1}\|_2
=\sqrt{\sum_i(\theta_{t,i}-\theta_{t-1,i})^2}
$$

저장된 실행 결과 일부는 다음과 같다.

| Epoch | Loss | parameter 변화량 |
|---:|---:|---:|
| 1 | 0.75977838 | 0.20615497 |
| 10 | 0.67387134 | 0.12946829 |
| 100 | 0.02529514 | 0.02763611 |
| 500 | 0.00228525 | 0.00497251 |
| 1000 | 0.00076190 | 0.00263150 |
| 2000 | 0.00022706 | 0.00149049 |

loss가 줄어드는 동안 parameter 변화량도 함께 작아졌다. “학습이 안정된다”는 말을 막연하게 쓰는 대신, 실제 update 크기를 기록해 확인한 셈이다.

---

## 헷갈리기 쉬운 포인트

#### 학습 중간에 `loss.detach()`를 하면 어떻게 될까?

```python
detached_loss = loss.detach()
detached_loss.backward()
```

`detached_loss`는 계산 그래프와 끊어져 있고 `requires_grad=False`이므로 정상적인 역전파를 할 수 없다. 숫자 기록은 `loss.item()`이나 `loss.detach()`로 해도 되지만, `backward()`는 연결된 원래 loss에 호출해야 한다.

#### `model.eval()`이 gradient를 꺼주나?

아니다. `eval()`은 Dropout과 BatchNorm 같은 layer의 동작 모드를 바꾼다. gradient 기록을 끄려면 `torch.no_grad()` 또는 `torch.inference_mode()`가 별도로 필요하다.

#### `torch.inference_mode()`는 `no_grad()`와 같은가?

둘 다 평가 시 gradient 기록을 막는다. `inference_mode()`는 추론 전용으로 더 많은 Autograd 추적을 비활성화해 추가 최적화가 가능하지만, 그 안에서 만든 텐서를 이후 gradient 계산에 섞는 용도에는 제약이 더 크다. 순수 평가라면 `inference_mode()`가 의도를 잘 드러낸다.

---

## 정리하며

학습 루프는 한 덩어리의 마법이 아니었다.

**forward가 그래프를 만들고 → loss가 그래프의 끝이 되고 → backward가 gradient를 `.grad`에 저장하고 → optimizer가 parameter를 갱신한다.**

그리고 `detach`, `clone`, `no_grad`는 모두 “값을 보관하거나 관찰하되 학습 그래프에는 넣지 않기 위한 도구”였다. 셋의 역할을 구분하고 나니 왜 학습 코드와 기록 코드가 다른 방식으로 작성되는지 이해할 수 있었다.

다음 글에서는 작은 XOR 텐서에서 벗어나 MNIST 60,000장을 `Dataset`과 `DataLoader`로 공급하는 과정을 따라간다.

[이전 글: PyTorch 02 - XOR로 이해하는 신경망 구조와 순전파](/2026/09/pytorch-02-xor-neural-network.html) · [다음 글: PyTorch 04 - MNIST Dataset과 DataLoader 이해하기](/2026/09/pytorch-04-mnist-dataset-dataloader.html)
