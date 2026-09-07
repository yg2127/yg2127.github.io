---
layout: post
title: "PyTorch 02 - XOR로 이해하는 신경망 구조와 순전파"
description: "nn.Module, Linear, forward와 BCEWithLogitsLoss를 XOR 분류 코드의 텐서 크기를 따라가며 정리"
date: 2026-09-07 09:10:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: pytorch, neural-network, xor, nn-module, linear-layer
comments: true
---

## 들어가며

PyTorch 예제는 흔히 `class Model(nn.Module)`로 시작한다. 처음에는 클래스를 왜 만들어야 하는지도 낯설었고, `nn.Linear(2, 4)`를 보고 **입력이 4개인데 왜 weight가 `(4, 2)`인지**도 헷갈렸다.

XOR은 이 질문들을 확인하기 좋은 작은 문제다. 데이터는 네 개뿐이지만 직선 하나로 분리할 수 없어서 은닉층과 비선형 활성화 함수가 꼭 필요하다.

> 한 줄 요약: **XOR의 네 샘플을 한 번에 넣으면 `Linear(2,4)`가 각 샘플의 feature 2개를 은닉 feature 4개로 바꾸고, Tanh가 선형 모델로는 만들 수 없는 결정경계를 만든다.**

---

## 1. XOR은 왜 직선 하나로 풀리지 않을까?

```python
X = torch.tensor(
    [[0, 0],
     [1, 0],
     [0, 1],
     [1, 1]],
    dtype=torch.float32,
    device=device,
)

y = torch.tensor(
    [0, 1, 1, 0],
    dtype=torch.float32,
    device=device,
).reshape(-1, 1)
```

| 입력 | 정답 |
|---|---:|
| `(0, 0)` | 0 |
| `(1, 0)` | 1 |
| `(0, 1)` | 1 |
| `(1, 1)` | 0 |

대각선으로 마주 보는 두 점끼리 같은 클래스다. 아무리 직선을 움직여도 한쪽에 0 두 개, 반대쪽에 1 두 개만 남길 수 없다. 그래서 입력을 새로운 feature 공간으로 옮긴 뒤 그 공간에서 분리해야 한다.

---

## 2. `nn.Module`로 모델 만들기

```python
import torch.nn as nn


class XOR(nn.Module):
    def __init__(self):
        super().__init__()
        self.hidden = nn.Linear(2, 4)
        self.output = nn.Linear(4, 1)

    def forward(self, x):
        hidden_output = torch.tanh(self.hidden(x))
        logits = self.output(hidden_output)
        return logits


model = XOR().to(device)
```

`nn.Module`은 PyTorch 모델의 기본 클래스다. 이를 상속하면 내부에 등록한 layer와 parameter를 PyTorch가 추적할 수 있다.

`super().__init__()`은 부모인 `nn.Module`의 초기화를 실행한다. 이 호출이 있어야 `self.hidden`, `self.output`에 넣는 layer들이 하위 모듈로 정상 등록되고, 이후 `model.parameters()`, `model.to(device)`, `state_dict()` 같은 기능이 작동한다.

---

## 3. `nn.Linear(2, 4)`가 만드는 것

`nn.Linear(in_features=2, out_features=4)`는 다음 연산을 수행한다.

$$
h=XW^\top+b
$$

PyTorch가 저장하는 parameter shape은 다음과 같다.

| parameter | shape | 의미 |
|---|---:|---|
| `hidden.weight` | `(4, 2)` | 은닉 unit 4개가 각각 입력 feature 2개를 읽음 |
| `hidden.bias` | `(4,)` | 은닉 unit마다 bias 하나 |
| `output.weight` | `(1, 4)` | 출력 unit 1개가 은닉 feature 4개를 읽음 |
| `output.bias` | `(1,)` | 출력 unit의 bias |

입력 배치 `X`가 `(4, 2)`인 이유는 **샘플 4개 × 샘플당 feature 2개**이기 때문이다. 여기서 앞의 4와 `hidden.weight` 앞의 4는 같은 숫자지만 의미가 전혀 다르다.

$$
(4,2)@(2,4)+(4,)=(4,4)
$$

- 첫 번째 4: batch에 들어 있는 샘플 수
- 두 번째 4: 각 샘플에서 만들어낸 은닉 feature 수

그래서 `hidden.weight`가 `(4, 4)`일 필요는 없다. batch 크기는 parameter의 shape을 결정하지 않는다.

---

## 4. `forward`를 shape로 읽기

```python
def forward(self, x):
    hidden_output = torch.tanh(self.hidden(x))
    logits = self.output(hidden_output)
    return logits
```

| 단계 | shape |
|---|---:|
| 입력 `x` | `(4, 2)` |
| `self.hidden(x)` | `(4, 4)` |
| `torch.tanh(...)` | `(4, 4)` |
| `self.output(...)` | `(4, 1)` |
| 반환 `logits` | `(4, 1)` |

모델 인스턴스를 `model(X)`처럼 호출하면 PyTorch의 `nn.Module.__call__()`이 내부에서 `forward(X)`를 실행한다. 사용자가 직접 `model.forward(X)`를 호출할 수도 있지만, hook과 같은 `nn.Module` 기능이 적용되는 `model(X)` 방식이 일반적이다.

---

## 5. Tanh가 필요한 이유

은닉층 두 개를 쌓았더라도 사이에 활성화 함수가 없다면 전체는 여전히 하나의 선형변환이다.

$$
W_2(W_1X+b_1)+b_2=(W_2W_1)X+(W_2b_1+b_2)
$$

층을 여러 개 둔 의미가 사라진다. `torch.tanh`는 원소별 비선형 변환을 넣어 입력을 휘어진 feature 공간으로 보낸다. 그 결과 출력층의 직선 하나가 원래 입력 공간에서는 XOR을 가르는 비선형 경계가 된다.

> “은닉층이 있어서 XOR을 푼다”보다 **“은닉층 사이에 비선형성이 있어서 XOR을 푼다”**가 더 정확한 표현이다.

---

## 6. 왜 출력에 Sigmoid가 없을까?

모델은 마지막 layer의 원시 점수인 `logits`를 그대로 반환한다.

```python
criterion = nn.BCEWithLogitsLoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.05)
```

`BCEWithLogitsLoss`는 Sigmoid와 Binary Cross Entropy를 하나로 합친 손실함수다. 모델 안에서 Sigmoid를 먼저 적용한 뒤 이 손실에 넣으면 Sigmoid를 두 번 적용하는 셈이 된다.

학습할 때는 logits를 손실함수에 바로 넣고, 확률이 필요한 평가 단계에서만 Sigmoid를 적용한다.

```python
model.eval()

with torch.inference_mode():
    logits = model(X)
    probability = torch.sigmoid(logits)
    prediction = (probability >= 0.5).to(torch.int64)
```

| 값 | shape | 범위·의미 |
|---|---:|---|
| `logits` | `(4, 1)` | 제한 없는 원시 점수 |
| `probability` | `(4, 1)` | 0~1 사이 클래스 1 확률 |
| `prediction` | `(4, 1)` | 0 또는 1 |
| `loss` | `()` | 기본 `reduction="mean"`으로 평균낸 스칼라 |

여기서 loss의 shape이 `(4, 1)`이라고 생각하기 쉽다. 개별 원소의 손실은 네 개지만, 기본 설정에서 평균을 내므로 최종 `loss`는 0차원 스칼라 텐서다.

---

## 7. 실제 학습 결과

2000 epoch 학습한 노트북에서는 다음 결과가 저장되었다.

```text
Input         Target     Probability     Prediction
[0, 0]             0       0.000003             0
[1, 0]             1       0.999754             1
[0, 1]             1       0.999753             1
[1, 1]             0       0.000412             0

Accuracy: 100.00%
```

출력 확률이 네 점을 모두 정확히 구분했다. 중요한 건 이 결과를 외운 것이 아니라, **입력 `(4,2)`가 은닉 표현 `(4,4)`를 거쳐 logits `(4,1)`로 바뀌는 과정**을 읽을 수 있게 된 것이다.

---

## 헷갈리기 쉬운 포인트

#### `in_features`, `out_features`를 안 써도 되나?

```python
nn.Linear(2, 4)
nn.Linear(in_features=2, out_features=4)
```

두 코드는 같다. 앞의 코드는 위치 인자로 전달하고, 뒤의 코드는 키워드 인자로 전달한다. 처음 구조를 읽을 때는 키워드를 쓰면 의미가 더 분명하다.

#### 흑백 분류라 출력이 두 개여야 하나?

XOR은 이진분류이므로 logit 하나만 출력해도 된다. Sigmoid를 통과한 값 하나가 클래스 1일 확률이고, 클래스 0의 확률은 `1 - p`다. 서로 배타적인 두 클래스에 logits 두 개와 CrossEntropyLoss를 쓰는 방식도 가능하지만, 현재 코드는 logit 하나와 BCEWithLogitsLoss를 택했다.

#### `model.train()`과 `model.eval()`이 XOR에서도 결과를 바꾸나?

현재 모델에는 Dropout이나 BatchNorm이 없어서 두 모드의 forward 계산은 같다. 그래도 학습·평가 단계의 의도를 명시하고, 나중에 그런 layer가 추가됐을 때 코드가 올바르게 동작하도록 모드를 전환한다.

---

## 정리하며

XOR 모델은 작지만 PyTorch 모델의 기본 골격을 모두 갖고 있다.

**`nn.Module` 상속 → `__init__`에서 layer 등록 → `forward`에서 데이터 흐름 정의 → logits 반환 → 손실함수로 학습 → 평가에서 확률로 변환**

다음 글에서는 이 모델이 어떻게 스스로 가중치를 바꾸는지 살펴본다. 특히 `loss.backward()`에 모델을 전달하지 않았는데도 어떻게 모든 layer의 gradient를 찾는지, `detach()`와 `no_grad()`는 왜 필요한지 정리한다.

[이전 글: PyTorch 01 - 텐서로 선형회귀를 직접 풀어보기](/2026/09/pytorch-01-tensor-linear-regression.html) · [다음 글: PyTorch 03 - backward부터 detach까지, Autograd 흐름 읽기](/2026/09/pytorch-03-autograd-training-loop.html)
