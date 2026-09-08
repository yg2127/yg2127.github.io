---
layout: post
title: "PyTorch 04 - MNIST Dataset과 DataLoader 이해하기"
description: "이미지 한 장이 (1, 28, 28) 텐서가 되고 DataLoader에서 (B, 1, 28, 28) 배치로 나오는 과정을 정리"
date: 2026-09-07 09:30:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: [pytorch, mnist, dataset, dataloader, batch]
series: pytorch
series_title: "PyTorch 직접 구현하기"
series_order: 4
comments: true
---

## 들어가며

XOR에서는 데이터 네 개를 직접 텐서로 만들었다. MNIST는 학습 이미지가 60,000장이니 같은 방식으로 관리하기 어렵다. 여기서 `Dataset`, `DataLoader`, `transform`이 한꺼번에 등장한다.

처음 코드를 읽으면서 가장 헷갈렸던 질문은 이것이었다.

> `train_dataset`이 transform을 통해 텐서로 바뀌었는데, 반복문에서는 왜 다시 `images`라는 변수로 받아도 괜찮지? 그리고 흑백 이미지인데 shape의 채널은 왜 1이지?

답은 **Dataset은 전체 데이터를 가진 객체이고, DataLoader가 거기서 꺼낸 텐서 배치를 원하는 변수 이름에 할당한다**는 데 있다. 변수 이름은 데이터의 타입을 바꾸지 않는다.

---

## 1. Dataset과 transform의 역할

```python
from torchvision import datasets, transforms

mnist_transform = transforms.ToTensor()

train_dataset = datasets.MNIST(
    root="./data",
    train=True,
    download=True,
    transform=mnist_transform,
)

test_dataset = datasets.MNIST(
    root="./data",
    train=False,
    download=True,
    transform=mnist_transform,
)
```

`datasets.MNIST`는 이미지와 정답을 보관하고, 인덱스로 샘플을 꺼내는 방법을 정의한 Dataset 객체다.

```python
image, target = train_dataset[0]

print(image.shape)   # torch.Size([1, 28, 28])
print(image.dtype)   # torch.float32
print(type(target))  # int
```

`transform=ToTensor()`는 샘플을 꺼내는 순간 PIL 이미지 또는 NumPy 배열을 PyTorch 텐서로 변환한다. 일반적인 8-bit 이미지라면 픽셀값도 0~255 정수에서 0~1 범위의 `float32`로 바뀐다.

Dataset 자체가 거대한 텐서 한 장으로 변하는 것은 아니다. **Dataset이 샘플을 반환할 때 적용할 변환 규칙을 가진다**고 보는 편이 정확하다.

---

## 2. 흑백 이미지의 채널은 왜 1인가

MNIST 이미지 한 장의 shape은 `(1, 28, 28)`이다.

| 축 | 크기 | 의미 |
|---|---:|---|
| `C` | 1 | 채널 수 |
| `H` | 28 | 세로 픽셀 수 |
| `W` | 28 | 가로 픽셀 수 |

흑백에 검정과 흰색 두 상태가 있으니 채널이 2일 것 같았지만, 채널 수는 가능한 색의 개수가 아니다. 흑백 이미지는 각 픽셀의 **밝기 값 하나**만 있으면 표현할 수 있어 채널이 1개다. 중간 회색까지 포함해도 한 채널의 숫자 크기로 나타낸다.

RGB 이미지는 한 픽셀을 빨강·초록·파랑 값 세 개로 표현하므로 채널이 3개다.

> `(1, 28, 28)`의 `1`은 “흑백 클래스가 하나”라는 뜻이 아니라 **픽셀마다 읽을 feature plane이 밝기 하나**라는 뜻이다.

---

## 3. DataLoader는 무엇을 더하는가

Dataset이 샘플 하나를 꺼내는 규칙이라면, DataLoader는 여러 샘플을 모아 mini-batch로 공급한다.

```python
from torch.utils.data import DataLoader

BATCH_SIZE = 128

train_loader = DataLoader(
    train_dataset,
    batch_size=BATCH_SIZE,
    shuffle=True,
)
```

```python
images, targets = next(iter(train_loader))

print(images.shape)   # torch.Size([128, 1, 28, 28])
print(targets.shape)  # torch.Size([128])
```

`iter(train_loader)`는 배치를 차례로 꺼내는 iterator를 만들고, `next(...)`는 그중 첫 배치를 반환한다. 반환값 `(images, targets)`를 두 변수로 unpacking한 것이다.

`images`라는 이름 때문에 이미지 객체로 되돌아간 것이 아니다. 변수 이름은 마음대로 정할 수 있고, 실제 값은 이미 transform을 거친 `torch.Tensor`다.

---

## 4. 배치 shape `(B, 1, 28, 28)` 읽기

DataLoader는 샘플 128개의 이미지 텐서를 새로운 batch 축에 쌓는다.

$$
(1,28,28)\times128\ \text{samples}
\longrightarrow (128,1,28,28)
$$

일반적으로 이미지 배치는 `(B, C, H, W)` 순서를 사용한다.

- `B`: 한 번에 모델로 보내는 샘플 수
- `C`: 이미지 채널 수
- `H`, `W`: 높이와 너비

target은 이미지마다 숫자 정답 하나이므로 `(B,)`, 즉 `(128,)`이다. 10개 클래스라고 해서 target shape이 `(128, 10)`인 것은 아니다. `CrossEntropyLoss`는 각 샘플의 클래스 인덱스 하나를 정답으로 받는다.

---

## 5. 학습용 loader와 평가용 loader를 나누는 이유

실습 코드에서는 같은 train Dataset으로 두 loader를 만들었다.

```python
train_loader = DataLoader(
    train_dataset,
    batch_size=128,
    shuffle=True,
)

train_eval_loader = DataLoader(
    train_dataset,
    batch_size=128,
    shuffle=False,
)

test_loader = DataLoader(
    test_dataset,
    batch_size=128,
    shuffle=False,
)
```

`train_loader`는 학습할 때 매 epoch 샘플 순서를 섞는다. 모델이 우연한 데이터 순서에 의존하는 것을 줄이고 mini-batch 구성을 바꾼다.

`train_eval_loader`는 학습 데이터 전체의 loss와 accuracy를 일정한 순서로 측정한다. 평가 값 자체는 순서와 무관하지만, 섞을 필요가 없다는 의도가 분명해지고 결과를 재현하기 쉽다. `test_loader`도 같은 이유로 `shuffle=False`를 사용한다.

---

## 6. shuffle도 재현하려면 Generator가 필요하다

```python
SEED = 42

random.seed(SEED)
np.random.seed(SEED)
torch.manual_seed(SEED)

if torch.cuda.is_available():
    torch.cuda.manual_seed_all(SEED)

train_generator = torch.Generator()
train_generator.manual_seed(SEED)

train_loader = DataLoader(
    train_dataset,
    batch_size=128,
    shuffle=True,
    generator=train_generator,
)
```

모델 초기 가중치만 고정해도 완전한 재현이 되지는 않는다. `shuffle=True`가 만드는 샘플 순서도 학습 결과에 영향을 주므로 DataLoader가 사용할 난수 생성기를 함께 고정한다.

CUDA의 완전한 결정론은 사용 연산과 환경에 따라 추가 설정이 필요하고 성능이 느려질 수도 있다. seed를 적었다고 모든 환경에서 bit 단위로 같은 결과가 보장되는 것은 아니라는 점도 기억해야 한다.

---

## 7. `num_workers`, `pin_memory`, `non_blocking`

GPU 학습 예제에서 자주 보이는 옵션들이다.

```python
use_pin_memory = device.type == "cuda"

train_loader = DataLoader(
    train_dataset,
    batch_size=128,
    shuffle=True,
    num_workers=2,
    pin_memory=use_pin_memory,
    generator=train_generator,
)
```

| 옵션 | 역할 |
|---|---|
| `num_workers` | Dataset에서 샘플을 읽고 transform하는 별도 worker process 수 |
| `pin_memory=True` | CPU batch를 page-locked memory에 준비해 CUDA 전송을 효율화 |
| `non_blocking=True` | 조건이 맞으면 CPU→GPU 복사를 비동기적으로 요청 |

```python
images = images.to(
    device,
    non_blocking=(device.type == "cuda"),
)
```

worker 수가 많다고 항상 빠른 것은 아니다. CPU core 수, 저장장치, transform 비용, 운영체제에 따라 적정값이 다르다. 노트북 환경에서 worker 생성이 멈추거나 느리다면 `0` 또는 `2`부터 확인하는 편이 안전하다.

---

## 8. 모델에 넣기 전에 shape 확인하기

학습을 시작하기 전에 배치 하나를 통과시켜 보면 shape 오류를 빠르게 발견할 수 있다.

```python
check_images, check_targets = next(iter(train_eval_loader))
check_images_device = check_images.to(device)

model.eval()

with torch.inference_mode():
    check_logits = model(check_images_device)

print("Input shape :", check_images.shape)
print("Logits shape:", check_logits.shape)
print("Target shape:", check_targets.shape)
```

저장된 실습 결과는 다음과 같았다.

```text
Input shape : torch.Size([128, 1, 28, 28])
Logits shape: torch.Size([128, 10])
Target shape: torch.Size([128])
```

이미지 128장이 들어가고, 각 이미지마다 숫자 0~9의 점수 10개가 나온다. 정답은 이미지마다 클래스 인덱스 하나다.

---

## 헷갈리기 쉬운 포인트

#### `images`로 받으면 tensor가 이미지 타입으로 바뀌나?

아니다. `images`는 변수 이름일 뿐이고, 값은 `torch.Tensor`다. `batch_images`나 `x`라고 이름을 바꿔도 동작은 같다.

#### Dataset에 transform을 넣으면 전체 60,000장이 즉시 변환되나?

일반적인 torchvision Dataset에서는 샘플을 `dataset[index]`로 꺼낼 때 transform이 적용된다. DataLoader는 필요한 인덱스의 샘플을 불러와 batch로 묶는다.

#### `ToTensor()`를 썼는데 MNIST 평균·표준편차로 다시 복원해야 하나?

`ToTensor()`만 사용했다면 이미지는 단순히 0~1 범위로 변환됐으므로 그대로 시각화하면 된다.

```python
image = images[0].squeeze(0)
plt.imshow(image.numpy(), cmap="gray")
```

`Normalize((0.1307,), (0.3081,))`까지 적용했을 때만 `image * 0.3081 + 0.1307`로 역정규화한다. 실습 코드처럼 `ToTensor()`만 적용한 뒤 역정규화를 한 번 더 하면 픽셀값이 달라진다.

---

## 정리하며

MNIST 데이터 흐름은 다음과 같다.

**원본 이미지 → Dataset이 인덱스로 샘플 선택 → transform이 `(1,28,28)` 텐서 생성 → DataLoader가 128개를 `(128,1,28,28)`로 묶음 → 학습 루프가 `images, targets`로 unpacking → device로 이동**

이제 `for images, targets in train_loader:` 한 줄 안에서 무엇이 나오는지 알 수 있다. 다음 글에서는 이 배치가 MLP를 통과하고, loss와 accuracy가 계산되는 전체 학습·평가 루프를 이어서 본다.

[이전 글: PyTorch 03 - backward부터 detach까지, Autograd 흐름 읽기](/2026/09/pytorch-03-autograd-training-loop.html) · [다음 글: PyTorch 05 - MNIST 학습과 평가 루프 완성하기](/2026/09/pytorch-05-mnist-training-evaluation.html)

## 참고

- [PyTorch DataLoader 공식 문서](https://docs.pytorch.org/docs/stable/data.html)
- [torchvision ToTensor 공식 문서](https://docs.pytorch.org/vision/stable/generated/torchvision.transforms.ToTensor.html)
