---
layout: post
title: "PyTorch 06 - Strided CNN과 Depthwise Separable CNN"
description: "Conv2d의 채널과 공간 크기를 계산하고, pooling을 학습 가능한 convolution으로 바꾸거나 depthwise와 pointwise로 분해하는 과정을 비교"
date: 2026-09-07 09:50:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: pytorch, cnn, conv2d, strided-convolution, depthwise-convolution
comments: true
---

## 들어가며

MLP는 MNIST 이미지를 `(784,)`로 펼쳐서 처리한다. 숫자의 모양을 이루는 인접 픽셀 관계도 함께 납작하게 만들어버린다. CNN은 작은 kernel을 이미지 위로 움직이며 지역 패턴을 먼저 찾는다.

Day5 실습에서는 기본 CNN을 그대로 만드는 대신 두 가지 변형을 구현했다.

1. Max Pooling을 stride가 2인 convolution으로 바꾼 `StridedCNN`
2. convolution을 depthwise와 pointwise로 나눈 `DepthwiseCNN`

코드를 작성하면서 `Conv2d`의 인자는 정리했지만, layer를 통과할 때마다 **왜 공간 크기가 26, 14, 12, 7로 바뀌는지**, 그리고 depthwise convolution을 썼는데도 **왜 전체 parameter가 더 많아졌는지**가 더 중요한 질문으로 남았다.

---

## 1. `Conv2d`에서 먼저 볼 네 가지

```python
nn.Conv2d(
    in_channels=1,
    out_channels=16,
    kernel_size=3,
    stride=1,
    padding=0,
)
```

| 인자 | 의미 |
|---|---|
| `in_channels` | 입력 feature map의 채널 수 |
| `out_channels` | 만들 filter 수이자 출력 채널 수 |
| `kernel_size` | filter의 공간 크기 |
| `stride` | filter가 한 번에 이동하는 간격 |
| `padding` | 입력 테두리에 추가하는 값의 폭 |
| `dilation` | kernel 원소 사이의 간격 |
| `groups` | 입력·출력 채널을 연결하는 묶음 수 |

공간 출력 크기는 한 축을 기준으로 다음처럼 계산한다.

$$
H_{out}=\left\lfloor
\frac{H_{in}+2P-D(K-1)-1}{S}+1
\right\rfloor
$$

여기서 $K$는 kernel, $S$는 stride, $P$는 padding, $D$는 dilation이다.

---

## 2. StridedCNN: downsampling도 학습시키기

```python
class StridedCNN(nn.Module):
    def __init__(self):
        super().__init__()

        self.conv1 = nn.Conv2d(1, 16, kernel_size=3)
        self.down1 = nn.Conv2d(
            16, 16, kernel_size=2, stride=2, padding=1
        )

        self.conv2 = nn.Conv2d(16, 32, kernel_size=3)
        self.down2 = nn.Conv2d(
            32, 32, kernel_size=2, stride=2, padding=1
        )

        self.relu = nn.ReLU()
        self.flatten = nn.Flatten()
        self.fc1 = nn.Linear(32 * 7 * 7, 256)
        self.fc2 = nn.Linear(256, 10)

    def forward(self, x):
        x = self.relu(self.conv1(x))
        x = self.down1(x)
        x = self.relu(self.conv2(x))
        x = self.down2(x)
        x = self.flatten(x)
        x = self.relu(self.fc1(x))
        return self.fc2(x)
```

Max Pooling은 정해진 영역의 최댓값을 남긴다. 여기서는 stride 2 convolution이 공간 크기를 줄인다. downsampling에 쓰는 filter weight도 loss를 통해 학습된다는 차이가 있다.

shape를 한 단계씩 계산하면 `fc1`의 `32 * 7 * 7`이 어디서 나왔는지 보인다.

| 단계 | 출력 shape |
|---|---:|
| 입력 | `(B, 1, 28, 28)` |
| `conv1`, $K=3$ | `(B, 16, 26, 26)` |
| `down1`, $K=2,S=2,P=1$ | `(B, 16, 14, 14)` |
| `conv2`, $K=3$ | `(B, 32, 12, 12)` |
| `down2`, $K=2,S=2,P=1$ | `(B, 32, 7, 7)` |
| `flatten` | `(B, 1568)` |
| `fc1` | `(B, 256)` |
| `fc2` | `(B, 10)` |

실습 코드에서 계산된 총 parameter는 414,202개였다.

---

## 3. Depthwise Separable Convolution

일반 convolution은 각 출력 filter가 모든 입력 채널을 함께 본다. 입력 채널이 $C_{in}$, 출력 채널이 $C_{out}$이면 weight 수는 bias를 제외하고 다음과 같다.

$$
C_{out}\times C_{in}\times K\times K
$$

Depthwise separable convolution은 이 일을 두 단계로 나눈다.

1. **Depthwise convolution**: 채널마다 공간 filter 하나를 적용
2. **Pointwise convolution**: $1\times1$ convolution으로 채널 정보를 결합

```python
self.depthwise_conv = nn.Conv2d(
    in_channels=32,
    out_channels=32,
    kernel_size=3,
    padding=1,
    dilation=2,
    groups=32,
)

self.pointwise_conv = nn.Conv2d(
    in_channels=32,
    out_channels=64,
    kernel_size=1,
)
```

`groups=32`이고 입력·출력 채널도 32이므로 각 filter는 입력 채널 하나만 본다. depthwise 단계에서는 채널 사이를 섞지 않고 공간 패턴만 추출한다. 이어지는 pointwise 단계가 32개 채널을 조합해 64개 출력 채널을 만든다.

| 방식 | weight 수 |
|---|---:|
| 일반 $3\times3$, 32→64 | $64\times32\times3\times3=18,432$ |
| Depthwise $3\times3$ | $32\times1\times3\times3=288$ |
| Pointwise $1\times1$ | $64\times32=2,048$ |
| 분리한 합계 | **2,336** |

이 convolution block만 보면 weight가 크게 줄어든다.

---

## 4. dilation을 넣은 DepthwiseCNN

```python
class DepthwiseCNN(nn.Module):
    def __init__(self):
        super().__init__()

        self.conv1 = nn.Conv2d(1, 32, kernel_size=3)
        self.depthwise_conv = nn.Conv2d(
            32, 32,
            kernel_size=3,
            padding=1,
            dilation=2,
            groups=32,
        )
        self.pointwise_conv = nn.Conv2d(
            32, 64, kernel_size=1
        )
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool2d(
            kernel_size=2,
            stride=2,
            padding=1,
        )
        self.flatten = nn.Flatten()
        self.fc1 = nn.Linear(64 * 7 * 7, 256)
        self.fc2 = nn.Linear(256, 10)

    def forward(self, x):
        x = self.relu(self.conv1(x))
        x = self.pool(x)

        x = self.depthwise_conv(x)
        x = self.pointwise_conv(x)
        x = self.relu(x)
        x = self.pool(x)

        x = self.flatten(x)
        x = self.relu(self.fc1(x))
        return self.fc2(x)
```

`dilation=2`인 $3\times3$ kernel의 실제 receptive field 크기는 $5\times5$다.

$$
K_{effective}=D(K-1)+1=2(3-1)+1=5
$$

따라서 padding이 1이어도 공간 크기가 유지되지 않는다.

| 단계 | 출력 shape |
|---|---:|
| 입력 | `(B, 1, 28, 28)` |
| `conv1` | `(B, 32, 26, 26)` |
| 첫 번째 pool | `(B, 32, 14, 14)` |
| dilated depthwise conv | `(B, 32, 12, 12)` |
| pointwise conv | `(B, 64, 12, 12)` |
| 두 번째 pool | `(B, 64, 7, 7)` |
| flatten | `(B, 3136)` |
| 출력 | `(B, 10)` |

---

## 5. Depthwise를 썼는데 왜 전체 모델은 더 클까?

노트북이 출력한 parameter 수는 다음과 같다.

| 모델 | 전체 parameter |
|---|---:|
| `StridedCNN` | 414,202 |
| `DepthwiseCNN` | 808,394 |

Depthwise block은 분명 효율적인데 전체 모델은 약 두 배가 됐다. 원인은 convolution이 아니라 `fc1`이다.

DepthwiseCNN은 마지막 feature map이 `(64, 7, 7)`이므로 flatten 결과가 3,136차원이다.

$$
3136\times256+256=803,072
$$

총 808,394개 중 803,072개가 `fc1`에 몰려 있다. 반면 StridedCNN은 `(32, 7, 7)`을 펼쳐 1,568차원이므로 `fc1` parameter가 401,664개다.

> Depthwise convolution은 **해당 convolution 연산**을 줄여준다. 모델의 출력 채널을 늘린 뒤 큰 feature map을 그대로 Linear layer에 연결하면 전체 parameter는 오히려 커질 수 있다.

이런 구조에서는 global average pooling처럼 공간 축을 먼저 요약하는 방식도 검토할 수 있다.

---

## 6. 저장된 학습 결과 비교

두 노트북 모두 5 epoch, AdamW, 같은 MNIST split을 사용해 다음 결과를 기록했다.

| 모델 | Final train acc | Final test acc | Train-test gap |
|---|---:|---:|---:|
| StridedCNN | 99.42% | **98.78%** | 0.64%p |
| DepthwiseCNN | 99.14% | 98.61% | **0.53%p** |

이 실행에서는 StridedCNN의 test accuracy가 0.17%p 높았다. 그러나 여기서 “strided convolution이 depthwise convolution보다 좋다”고 결론 내릴 수는 없다.

- 입력·출력 채널 수가 서로 다르다.
- downsampling 방식도 다르다.
- 전체 parameter 수가 약 두 배 차이 난다.
- seed 하나, 실행 한 번의 결과다.

비교하려면 채널 수와 parameter budget을 맞추고 여러 seed에서 평균과 분산을 확인해야 한다. 현재 결과는 **두 구현이 정상적으로 학습됐다는 관찰값**으로 보는 것이 적절하다.

---

## 7. weight 변화량 추적하기

실습에서는 각 layer의 현재 weight를 초기값·직전 epoch와 비교했다.

```python
initial_weights = {
    name: parameter.detach().cpu().clone()
    for name, parameter in model.named_parameters()
    if name.endswith(".weight")
}
```

상대 변화율은 weight 크기가 서로 다른 layer도 비교하기 위해 기준 weight의 norm으로 나눈다.

$$
\text{relative change}(\%)=
100\times\frac{\|W_t-W_{ref}\|_2}
{\|W_{ref}\|_2+\epsilon}
$$

직전 epoch를 기준으로 하면 update가 점점 안정되는지 볼 수 있고, 초기값을 기준으로 하면 학습 전체에서 어느 layer가 많이 이동했는지 볼 수 있다. 단, norm 변화량이 크다고 그 layer가 더 중요하다는 뜻은 아니다. parameter 규모와 최적화 환경을 함께 봐야 한다.

---

## 헷갈리기 쉬운 포인트

#### `MaxPool2d`에 input size와 output size를 적지 않는 이유는?

Pooling layer는 입력 feature map에 같은 규칙을 적용하므로 Linear처럼 고정된 `in_features`, `out_features`를 받을 필요가 없다. kernel, stride, padding과 실제 입력 크기로 출력 공간 크기가 계산된다.

#### `groups`는 채널을 groups개씩 묶는다는 뜻인가?

정확히는 입력·출력 채널의 연결을 `groups`개의 독립된 묶음으로 나눈다. `groups=1`이면 모든 입력 채널이 모든 출력 채널에 연결된다. `groups=in_channels`이고 `out_channels=in_channels`이면 채널마다 filter 하나를 적용하는 depthwise convolution이 된다.

#### `padding=1`이면 항상 크기가 유지되나?

아니다. kernel, stride, dilation을 함께 봐야 한다. 특히 `kernel_size=3`, `dilation=2`의 effective kernel은 5이므로 stride 1에서 크기를 유지하려면 일반적으로 padding 2가 필요하다.

#### logits에 softmax를 넣고 CrossEntropyLoss를 써야 하나?

학습 시에는 넣지 않는다. 모델은 logits를 반환하고 `CrossEntropyLoss`에 그대로 전달한다. 확률을 표시할 때만 softmax를 적용한다.

---

## 정리하며

CNN 구조를 읽을 때 layer 이름만 보면 연결이 잘 보이지 않았다. 이번 실습에서는 각 단계의 `(C,H,W)`를 직접 계산하자 구조가 선명해졌다.

- StridedCNN은 downsampling filter까지 학습한다.
- Depthwise convolution은 채널별 공간 연산을, pointwise convolution은 채널 결합을 담당한다.
- dilation은 parameter 수를 늘리지 않고 receptive field를 넓힌다.
- 전체 parameter는 convolution 하나의 효율보다 마지막 feature map과 Linear layer에서 더 크게 좌우될 수 있다.

결국 `Conv2d`를 이해하는 가장 확실한 방법도 shape를 끝까지 따라가는 것이었다. `(B,C,H,W)`가 어디서 어떻게 바뀌는지만 놓치지 않으면 `Flatten → Linear`의 입력 크기도 계산으로 확인할 수 있다.

[이전 글: PyTorch 05 - MNIST 학습과 평가 루프 완성하기](/2026/09/pytorch-05-mnist-training-evaluation.html)

## 참고

- [Conv2d 공식 문서](https://docs.pytorch.org/docs/stable/generated/torch.nn.Conv2d.html)
- [MaxPool2d 공식 문서](https://docs.pytorch.org/docs/stable/generated/torch.nn.MaxPool2d.html)
- [MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications](https://arxiv.org/abs/1704.04861)
