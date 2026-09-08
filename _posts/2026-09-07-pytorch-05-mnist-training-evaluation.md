---
layout: post
title: "PyTorch 05 - MNIST 학습과 평가 루프 완성하기"
description: "MLP의 hidden dimension부터 CrossEntropyLoss, train/eval, 평균 loss와 accuracy, Matplotlib 기록까지 전체 학습 흐름을 정리"
date: 2026-09-07 09:40:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: [pytorch, mnist, mlp, training-loop, evaluation]
series: pytorch
series_title: "PyTorch 직접 구현하기"
series_order: 5
comments: true
---

## 들어가며

Dataset과 DataLoader를 만들고 나면 드디어 학습 루프가 나온다. 그런데 이쯤부터 코드 한 줄의 의미는 알면서도 전체 흐름을 놓치기 쉬웠다.

- `hidden_dim`을 왜 인자로 받을까? 그냥 256이라고 쓰면 안 되나?
- logits는 확률인가?
- `model.eval()`만 호출하면 gradient도 꺼지나?
- batch마다 나온 평균 loss를 왜 다시 batch 크기와 곱할까?

이번 글에서는 MNIST MLP의 입력부터 최종 평가까지를 **텐서 shape와 상태 변화** 두 축으로 따라간다.

> 한 줄 요약: **학습은 mini-batch마다 parameter를 바꾸는 과정이고, 평가는 parameter를 고정한 채 모든 샘플의 합계에서 loss와 accuracy를 계산하는 과정이다.**

---

## 1. 두 개의 은닉층을 가진 MLP

```python
class MLP2Layer(nn.Module):
    def __init__(self, hidden_dim_1=256, hidden_dim_2=256):
        super().__init__()

        self.fc1 = nn.Linear(28 * 28, hidden_dim_1)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(hidden_dim_1, hidden_dim_2)
        self.output = nn.Linear(hidden_dim_2, 10)

    def forward(self, x):
        x = torch.flatten(x, start_dim=1)
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        return self.output(x)
```

MNIST 배치 `(B, 1, 28, 28)`를 Linear layer에 넣기 위해 한 이미지의 채널·높이·너비를 펼친다.

```python
x = torch.flatten(x, start_dim=1)
```

`start_dim=1`이 중요하다. 0번 축인 batch는 보존하고 나머지만 합친다.

$$
(B,1,28,28)\rightarrow(B,784)
$$

이후 shape 흐름은 다음과 같다.

| 단계 | 출력 shape |
|---|---:|
| 입력 이미지 | `(B, 1, 28, 28)` |
| `flatten(start_dim=1)` | `(B, 784)` |
| `fc1 + ReLU` | `(B, 256)` |
| `fc2 + ReLU` | `(B, 256)` |
| `output` | `(B, 10)` |

---

## 2. `hidden_dim`을 인자로 받는 이유

```python
model = MLP2Layer(
    hidden_dim_1=256,
    hidden_dim_2=256,
).to(device)
```

256을 클래스 안에 직접 적어도 현재 모델은 동작한다. 인자로 받는 이유는 **모델 구조와 실험 설정을 분리하기 위해서**다.

```python
small_model = MLP2Layer(128, 64)
wide_model = MLP2Layer(512, 512)
```

클래스 코드를 수정하지 않고도 모델 폭을 바꿀 수 있고, 설정값을 로그에 남기거나 하이퍼파라미터 실험을 자동화하기 쉽다. 기본값 `=256`이 있으므로 아무 인자도 주지 않으면 원래 구조가 유지된다.

저장된 모델의 parameter는 다음과 같다.

| parameter | shape |
|---|---:|
| `fc1.weight` | `(256, 784)` |
| `fc1.bias` | `(256,)` |
| `fc2.weight` | `(256, 256)` |
| `fc2.bias` | `(256,)` |
| `output.weight` | `(10, 256)` |
| `output.bias` | `(10,)` |

총 학습 가능 parameter는 269,322개다.

---

## 3. logits와 CrossEntropyLoss

```python
criterion = nn.CrossEntropyLoss()
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=0.001,
)
```

모델 출력 `(B, 10)`의 각 값은 확률이 아니라 **logit**, 즉 클래스별 원시 점수다. 음수일 수도 있고 합이 1일 필요도 없다.

`CrossEntropyLoss`는 logits와 클래스 인덱스 target을 받는다.

- 입력 logits: `(B, 10)`의 floating-point tensor
- 정답 targets: `(B,)`의 integer tensor, 각 값은 0~9
- 출력 loss: 기본 평균 reduction이 적용된 scalar tensor

학습 전에 softmax를 직접 적용할 필요가 없다. `CrossEntropyLoss`가 내부에서 log-softmax와 negative log-likelihood에 해당하는 계산을 안정적으로 결합한다.

확률이 필요한 추론 단계에서만 다음처럼 변환한다.

```python
probabilities = torch.softmax(logits, dim=1)
predictions = logits.argmax(dim=1)
```

`argmax`는 softmax 전후에 같은 클래스 인덱스를 반환하므로 예측 클래스만 필요할 때는 logits에 바로 호출할 수 있다.

---

## 4. mini-batch 학습 루프

```python
history = {
    "train_loss": [],
    "train_acc": [],
    "test_loss": [],
    "test_acc": [],
}

for epoch in range(1, EPOCHS + 1):
    model.train()

    for images, targets in train_loader:
        images = images.to(
            device,
            non_blocking=(device.type == "cuda"),
        )
        targets = targets.to(
            device,
            non_blocking=(device.type == "cuda"),
        )

        optimizer.zero_grad(set_to_none=True)
        logits = model(images)
        loss = criterion(logits, targets)
        loss.backward()
        optimizer.step()
```

한 epoch에 60,000장을 모두 사용하지만 parameter update는 60,000장을 한 번에 본 뒤 일어나는 것이 아니다. batch size가 128이라면 대략 469개의 mini-batch마다 update가 한 번씩 일어난다.

**데이터 이동 → gradient 초기화 → forward → loss → backward → update**가 mini-batch마다 반복된다.

---

## 5. 평가 함수는 왜 따로 만들까?

```python
def evaluate(model, data_loader, criterion, device):
    model.eval()

    total_loss = 0.0
    total_correct = 0
    total_samples = 0

    with torch.inference_mode():
        for images, targets in data_loader:
            images = images.to(device)
            targets = targets.to(device)

            logits = model(images)
            loss = criterion(logits, targets)
            predictions = logits.argmax(dim=1)

            batch_size = targets.size(0)
            total_loss += loss.item() * batch_size
            total_correct += (
                predictions == targets
            ).sum().item()
            total_samples += batch_size

    average_loss = total_loss / total_samples
    accuracy = total_correct / total_samples
    return average_loss, accuracy
```

평가에서는 parameter를 바꾸지 않는다. `model.eval()`로 layer 동작을 평가 모드로 바꾸고, `torch.inference_mode()`로 Autograd 기록도 끈다. 둘은 서로 다른 역할이라 함께 사용한다.

---

## 6. 평균 loss에 batch 크기를 다시 곱하는 이유

기본 `CrossEntropyLoss`는 현재 batch의 샘플 loss를 평균낸 값 하나를 반환한다. 모든 batch의 평균을 단순히 다시 평균내면 마지막 batch가 작을 때 각 샘플의 가중치가 달라진다.

예를 들어 전체 샘플 수가 batch size로 나누어떨어지지 않으면 마지막 batch에는 더 적은 샘플이 들어 있다. 그래서 batch 평균을 먼저 합계로 되돌린다.

```python
total_loss += loss.item() * batch_size
```

모든 합계를 모은 뒤 전체 샘플 수로 나눈다.

```python
average_loss = total_loss / total_samples
```

이렇게 해야 데이터셋 전체의 **샘플당 평균 loss**가 된다.

accuracy도 같은 원리로 맞은 개수를 먼저 더한다.

```python
total_correct += (predictions == targets).sum().item()
accuracy = total_correct / total_samples
```

---

## 7. epoch 결과 기록하기

```python
train_loss, train_acc = evaluate(
    model, train_eval_loader, criterion, device
)
test_loss, test_acc = evaluate(
    model, test_loader, criterion, device
)

history["train_loss"].append(train_loss)
history["train_acc"].append(train_acc)
history["test_loss"].append(test_loss)
history["test_acc"].append(test_acc)
```

저장된 5 epoch 실행 결과는 다음과 같았다.

| Epoch | Train loss | Train acc | Test loss | Test acc |
|---:|---:|---:|---:|---:|
| 1 | 0.1481 | 95.75% | 0.1515 | 95.38% |
| 2 | 0.0780 | 97.61% | 0.0948 | 96.91% |
| 3 | 0.0581 | 98.17% | 0.0911 | 97.14% |
| 4 | 0.0403 | 98.76% | **0.0776** | **97.56%** |
| 5 | **0.0335** | **98.91%** | 0.0801 | 97.37% |

Train 성능은 마지막까지 좋아졌지만 Test loss와 accuracy는 4 epoch에서 가장 좋고 5 epoch에 소폭 나빠졌다. 한 번의 실행만으로 과적합을 단정할 수는 없지만, **훈련 성능만 보고 마지막 epoch를 무조건 선택하면 안 되는 신호**는 확인할 수 있다.

---

## 8. 그래프로 확인할 때 생긴 실수

```python
epoch_axis = np.arange(1, len(history["train_loss"]) + 1)

fig, axes = plt.subplots(1, 2, figsize=(12, 4))

axes[0].plot(
    epoch_axis,
    history["train_loss"],
    marker="o",
    label="Train Loss",
)
axes[0].plot(
    epoch_axis,
    history["test_loss"],
    marker="o",
    label="Test Loss",
)
axes[0].legend()

axes[1].plot(
    epoch_axis,
    np.array(history["train_acc"]) * 100,
    marker="o",
    label="Train Accuracy",
)
axes[1].plot(
    epoch_axis,
    np.array(history["test_acc"]) * 100,
    marker="o",
    label="Test Accuracy",
)
axes[1].legend()

plt.tight_layout()
plt.show()
```

실습 중에는 x축을 256개로 만들고 y값은 5개만 넣어 shape 오류가 났다. x축 길이는 hidden dimension이 아니라 기록한 epoch 수와 같아야 한다.

또 `plt.subplots(1, 2)`의 `axes`는 Axes 하나가 아니라 NumPy 배열이다. 따라서 `axes.legend()`가 아니라 `axes[0].legend()`, `axes[1].legend()`처럼 각 subplot에 호출해야 한다.

---

## 헷갈리기 쉬운 포인트

#### logits 10개는 클래스별 확률인가?

아니다. 서로 비교하기 위한 원시 점수다. 확률이 필요하면 `softmax(logits, dim=1)`을 적용한다.

#### 평가할 때 왜 `model.eval()`과 `inference_mode()`를 둘 다 쓰나?

`eval()`은 Dropout·BatchNorm 같은 layer의 행동을 바꾸고, `inference_mode()`는 gradient 기록을 끈다. 하나가 다른 하나를 대신하지 않는다.

#### `hidden_dim=256`을 인자로 받는 것이 항상 더 좋은가?

한 번만 쓰는 고정 모델이라면 상수로 적어도 된다. 여러 구조를 비교하거나 설정을 재사용할 때 인자로 분리한 장점이 커진다. 중요한 건 숫자를 어디에 적느냐보다 **layer 사이의 입력·출력 차원이 일치하도록 연결하는 것**이다.

#### 이미지 시각화에서 MNIST 평균과 표준편차를 왜 쓰나?

transform에 `Normalize((0.1307,), (0.3081,))`를 적용했을 때 원래 밝기로 되돌리기 위해 쓴다. 현재 실습처럼 `ToTensor()`만 사용했다면 역정규화 없이 바로 그려야 한다.

---

## 정리하며

MNIST 학습 코드를 한 문장으로 읽으면 다음과 같다.

**DataLoader가 `(B,1,28,28)` 배치를 제공하고 → MLP가 `(B,10)` logits를 만들고 → CrossEntropyLoss가 scalar loss를 만들고 → backward가 gradient를 계산하고 → AdamW가 parameter를 바꾸며 → 평가 함수가 전체 샘플 기준 loss와 accuracy를 계산한다.**

각 줄의 역할뿐 아니라 shape와 상태가 언제 바뀌는지 함께 보면 긴 학습 루프도 훨씬 덜 복잡해진다.

다음 글에서는 이미지의 공간 구조를 버리고 펼쳐버리는 MLP 대신, `Conv2d`로 공간 정보를 처리한다. Max Pooling을 strided convolution으로 바꾸는 모델과 depthwise·pointwise convolution을 분리한 모델까지 비교한다.

[이전 글: PyTorch 04 - MNIST Dataset과 DataLoader 이해하기](/2026/09/pytorch-04-mnist-dataset-dataloader.html) · [다음 글: PyTorch 06 - Strided CNN과 Depthwise Separable CNN](/2026/09/pytorch-06-cnn-convolutions.html)

## 참고

- [CrossEntropyLoss 공식 문서](https://docs.pytorch.org/docs/stable/generated/torch.nn.CrossEntropyLoss.html)
- [Module.eval 공식 문서](https://docs.pytorch.org/docs/stable/generated/torch.nn.Module.html#torch.nn.Module.eval)
- [inference_mode 공식 문서](https://docs.pytorch.org/docs/stable/generated/torch.autograd.grad_mode.inference_mode.html)
