---
layout: post
title: "PyTorch 01 - 텐서로 선형회귀를 직접 풀어보기"
description: "reshape, cat, ones_like, lstsq와 행렬곱을 따라가며 텐서의 shape가 실제 계산에서 어떤 역할을 하는지 정리"
date: 2026-09-07 09:00:00 +0900
category: "Deep Learning"
subcategory: "PyTorch"
tags: pytorch, tensor, linear-regression, least-squares, matrix
comments: true
---

## 들어가며

PyTorch를 처음 보면 `torch.tensor`, `reshape`, `cat`, `item`처럼 짧은 메소드가 계속 등장한다. 각각의 정의는 금방 찾을 수 있었지만, 정작 코드를 읽을 때는 **왜 여기서 텐서 모양을 바꾸고, 왜 1로 채운 열을 붙이는지**가 잘 이어지지 않았다.

이번 실습에서는 신경망부터 만들지 않고, 10개의 점에 가장 잘 맞는 직선을 최소제곱법으로 구했다. 덕분에 PyTorch 코드의 기본 문법이 결국 하나의 행렬식으로 모이는 과정을 볼 수 있었다.

> 한 줄 요약: **입력 벡터를 설계행렬 $X=[1,x]$로 만들면, `torch.linalg.lstsq`와 행렬곱만으로 선형회귀를 구현할 수 있다.**

---

## 1. 데이터를 열벡터로 만들기

먼저 10개의 $x$, $y$ 값을 GPU의 `float64` 텐서로 만든다.

```python
import torch

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

x = torch.tensor(
    [0.8147, 0.9058, 0.1270, 0.9134, 0.6324,
     0.0975, 0.2785, 0.5469, 0.9575, 0.9649],
    dtype=torch.float64,
    device=device,
).reshape(-1, 1)

y = torch.tensor(
    [0.8576, 0.9706, 0.2572, 0.8854, 0.8003,
     0.1419, 0.4218, 0.9157, 0.7922, 0.9595],
    dtype=torch.float64,
    device=device,
).reshape(-1, 1)
```

`torch.tensor()`에는 행이나 열을 정하는 `dim` 인자가 없다. 먼저 데이터로 1차원 텐서를 만든 뒤 `reshape(-1, 1)`로 열벡터를 만든다.

| 코드 | shape | 의미 |
|---|---:|---|
| `torch.tensor([...])` | `(10,)` | 원소 10개인 1차원 텐서 |
| `.reshape(-1, 1)` | `(10, 1)` | 10행 1열의 열벡터 |

여기서 `-1`은 “이 축의 크기는 전체 원소 수에 맞춰 자동으로 계산해 달라”는 뜻이다. 열을 1개로 고정했으므로 행은 자동으로 10이 된다.

---

## 2. 왜 1로 채운 열을 붙일까?

구하려는 직선은 다음과 같다.

$$
\hat y = b + wx
$$

모든 샘플을 한 번에 계산하려면 이를 행렬곱으로 바꿔야 한다.

$$
\begin{bmatrix}
\hat y_1 \\
\hat y_2 \\
\vdots \\
\hat y_{10}
\end{bmatrix}
=
\begin{bmatrix}
1 & x_1 \\
1 & x_2 \\
\vdots & \vdots \\
1 & x_{10}
\end{bmatrix}
\begin{bmatrix}
b \\
w
\end{bmatrix}
$$

첫 번째 열의 `1`이 절편 $b$와 곱해지고, 두 번째 열의 $x$가 기울기 $w$와 곱해진다. 코드에서는 `ones_like`와 `cat`으로 이 설계행렬을 만든다.

```python
X = torch.cat([torch.ones_like(x), x], dim=1)
```

- `torch.ones_like(x)`는 `x`와 shape, dtype, device가 같은 1 텐서를 만든다.
- `dim=1`은 두 텐서를 열 방향으로 붙인다.
- 따라서 `X.shape`은 `(10, 2)`가 된다.

처음에는 `feat = [ones, x]` 같은 리스트만으로도 계산할 수 있을 것 같았다. 하지만 리스트는 텐서 두 개를 담은 Python 컨테이너일 뿐이다. `lstsq`에 넣으려면 열들이 결합된 **하나의 2차원 텐서**가 필요하다.

---

## 3. 최소제곱해 구하기

관측값이 직선 위에 정확히 놓이지 않으므로 $X\theta=y$를 완벽하게 만족하는 해는 없다. 대신 잔차 제곱합을 가장 작게 만드는 $	heta=[b,w]^\top$를 구한다.

$$
\theta^*=\underset{\theta}{\arg\min}\;\|X\theta-y\|_2^2
$$

```python
weight = torch.linalg.lstsq(X, y).solution

intercept = weight[0].item()
slope = weight[1].item()
y_pred = X @ weight
```

shape만 따라가도 연산이 보인다.

$$
(10,2)@(2,1)=(10,1)
$$

`weight`는 `(2, 1)` 텐서이고, 첫 번째 원소가 절편, 두 번째 원소가 기울기다. `X @ weight`는 10개 입력의 예측값을 한 번에 계산한다.

> `lstsq`가 차원을 알아서 만들어준다고 생각하기보다, **입력 `X`의 열 개수가 구해야 할 계수의 개수를 결정한다**고 이해하는 편이 정확하다.

---

## 4. `item()`은 언제 쓰는가

```python
weight[0]
# tensor([0.1941], device='cuda:0', dtype=torch.float64)

weight[0].item()
# 0.194080159812...
```

`weight[0]`은 원소가 하나여도 여전히 텐서다. `.item()`은 원소가 하나인 텐서를 Python 숫자로 바꾼다. 출력 문자열에 넣거나 일반 Python 값이 필요한 순간에 유용하다.

반대로 이후 계산을 계속 PyTorch에서 수행한다면 굳이 일찍 `item()`을 호출할 필요가 없다. 텐서에서 Python 숫자로 나오는 순간 autograd 연결과 device 정보도 함께 사라지기 때문이다.

---

## 5. 예측 결과 평가하기

실습에서는 RMSE와 결정계수 $R^2$도 텐서 연산으로 직접 계산했다.

```python
rmse = torch.sqrt(torch.mean((y - y_pred) ** 2)).item()

ss_res = torch.sum((y - y_pred) ** 2)
ss_tot = torch.sum((y - torch.mean(y)) ** 2)
r2 = (1 - ss_res / ss_tot).item()

print(f"회귀식: y = {slope:.6f}x {intercept:+.6f}")
print(f"R2 = {r2:.6f}")
print(f"RMSE = {rmse:.6f}")
```

저장된 노트북의 실행 결과는 다음과 같았다.

```text
회귀식: y = 0.811304x +0.194080
R2 = 0.833847
RMSE = 0.118827
```

---

## 6. 다항회귀로 확장해보니

직선의 설계행렬이 `[1, x]`였다면 $d$차 다항회귀는 `[1, x, x², ..., xᵈ]`로 확장하면 된다.

```python
def make_poly_features(input_x, degree):
    features = [torch.ones_like(input_x)]

    for power in range(1, degree + 1):
        features.append(input_x ** power)

    return torch.cat(features, dim=1)


def fit_poly(input_x, target, degree):
    X_poly = make_poly_features(input_x, degree)
    return torch.linalg.lstsq(X_poly, target).solution
```

차수가 $d$이면 설계행렬은 `(10, d+1)`, 가중치는 `(d+1, 1)`이다. 이 실험에서 차수를 1부터 8까지 높이자 훈련 RMSE는 계속 줄었지만, LOOCV RMSE는 2차 이후 다시 커졌다.

| 차수 | Train RMSE | LOOCV RMSE |
|---:|---:|---:|
| 1 | 0.118827 | 0.144755 |
| 2 | 0.066986 | **0.097774** |
| 4 | 0.063891 | 0.162496 |
| 6 | 0.052451 | 0.357053 |
| 8 | **0.002012** | 23.941179 |

8차식은 훈련점 10개를 거의 완벽하게 통과하지만, 한 점을 빼고 다시 맞추면 오차가 폭발했다. **훈련 오차가 작다는 사실만으로 좋은 모델이라고 말할 수 없는 이유**가 숫자로 드러난다.

---

## 헷갈리기 쉬운 포인트

#### `torch.tensor([...], dim=1)`로 열벡터를 만들 수 있나?

그렇게 쓸 수 없다. `torch.tensor`는 데이터를 받아 텐서를 생성하고, 축 모양은 `reshape`, `unsqueeze` 등으로 바꾼다.

#### `weight`의 크기는 PyTorch가 임의로 정하나?

`X`가 `(N, P)`, `y`가 `(N, K)`라면 해의 shape은 `(P, K)`다. 이 실습에서는 입력 열이 `[1, x]` 두 개이므로 계수도 절편과 기울기 두 개다.

#### CUDA 텐서를 바로 Matplotlib에 넣어도 되나?

Matplotlib은 CUDA 텐서를 직접 처리하지 못한다. 그래프를 그릴 때만 다음처럼 CPU의 NumPy 배열로 옮긴다.

```python
x_plot = x.detach().cpu().squeeze(1).numpy()
```

---

## 정리하며

이번 실습에서 가장 중요했던 것은 메소드 이름보다 shape였다.

**열벡터 `(10,1)` → 1열을 붙인 설계행렬 `(10,2)` → 최소제곱 계수 `(2,1)` → 행렬곱으로 예측 `(10,1)`**

이 흐름이 보이기 시작하면 `reshape`, `cat`, `ones_like`, `lstsq`, `@`, `item()`이 따로 놀지 않는다. 모두 $X\theta=\hat y$를 코드로 옮기기 위한 단계였다.

다음 글에서는 선형 경계 하나로 풀 수 없는 XOR 문제를 `nn.Module`로 구현한다.

[다음 글: PyTorch 02 - XOR로 이해하는 신경망 구조와 순전파](/2026/09/pytorch-02-xor-neural-network.html)
