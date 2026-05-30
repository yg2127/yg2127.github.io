---
layout: post
title: "CNN 3강 - 손실 함수와 최적화"
description: "SVM Loss는 왜 max를 쓰는지, Cross Entropy의 log는 뭘 의미하는지 — 수식이 직관으로 바뀌는 순간"
date: 2026-02-21
category: "Deep Learning"
subcategory: "CNN"
tags: deep-learning, cnn, loss-function, optimization
comments: true
---

## 들어가며

지난 강의에서 Linear Classifier로 $f(x, W) = Wx + b$ 를 배웠는데, 핵심 질문이 남았다 — "좋은 W는 어떻게 찾는가?" 이번 강의에서는 W가 얼마나 나쁜지를 측정하는 <mark>Loss function</mark>, 그리고 그걸 줄이는 <mark>Optimization</mark>을 배웠다.

솔직히 말하면, 수식이 여러 개 나오면서 처음에 좀 멘붕이었다. 특히 ~~SVM Loss랑 Softmax Loss는 그냥 둘 다 똑같은 분류 손실~~ 아닌가 싶었는데, 각각 뭘 의미하는지·왜 다른 함수를 쓰는지가 한참 헷갈렸다. 정리하면서 겨우 감이 잡혔다.

> 이번 강의를 한 줄로 요약하면: **Loss function으로 W의 품질을 측정하고, Gradient Descent로 W를 개선한다.**

---

## Multiclass SVM Loss (Hinge Loss)

### 수식과 의미

주어진 예시 $(x_{i},y_{i})\ x_i = image,\ y_i = label$ 에 대해 SVM Loss는:

$$L_{i} = \sum\limits_{j \ne y_{i}}max(0, s_{j}-s_{y_{i}}+1)$$

각 기호의 의미는 다음과 같다.

| 기호 | 의미 |
|---|---|
| $s_j$ | 틀린 클래스 $j$ 의 예측 점수 |
| $s_{y_i}$ | 정답 클래스의 예측 점수 |
| $+1$ | 마진(margin) — 정답이 오답보다 최소 이만큼은 높아야 함 |

처음 이 수식을 봤을 때 "max(0, ...)은 뭐지?" 싶었는데, 의미를 풀어보면 간단하다.

> **정답 점수가 오답 점수보다 1 이상 높으면 loss가 0이고, 그렇지 않으면 그 차이만큼 벌점을 준다.**

"1 이상 이기면 OK, 아니면 패널티" 같은 느낌이다.

### 초기 loss로 디버깅하기

가중치가 처음에 0에 가깝게 초기화되면 $s_j$와 $s_{y_i}$ 모두 0에 근사하게 되니까, max 함수에서 나오는 값들이 1에 근사하게 된다. 이런 식으로 초기 loss 값을 예측해보는 게 디버깅에 유용하다.

---

## 전체 손실 함수와 정규화(Regularization)

### 데이터 손실 + 정규화 항

전체 데이터셋에 대한 손실 함수는:

$$L=\frac{1}{N}\sum\limits^{N}_{i=1}\sum\limits_{j \ne y_{i}}max(0, f(x_{i};W)_{j} - f(x_{i};W_{y_{i}})+1) + \lambda R(W)$$

여기서 $\lambda R(W)$가 <mark>정규화(규제) 항</mark>이다. 앞부분은 데이터에 얼마나 잘 맞는지(data loss), 뒷부분은 모델이 얼마나 단순한지(regularization)를 나타낸다.

### 정규화의 종류

- L1 regularization $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l}W^{2}_{k,l}$
- L2 regularization $\rightarrow$ $\sum\limits_{k}\sum\limits_{l}\|W_{k,l}\|$
- Elastic net(L1+L2) $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l} \beta W^{2}_{k,l}+\|W_{k,l}\|$
- Max norm regularization
- Dropout

### 왜 정규화가 필요한가 — 예시로 이해하기

처음에 "왜 정규화가 필요하지?"라고 생각했는데, 예시를 보니 이해가 됐다. $w_{1} = [1,0,0,0]$과 $w_{2}=[0.25,0.25,0.25,0.25]$가 있을 때, $x = [1,1,1,1]$에 대해 $w^{\top}_{1}x = w^{\top}_{2}x = 1$로 출력값이 같다.

| 가중치 | 특성 사용 방식 | 정규화의 선호 |
|---|---|---|
| $w_1 = [1,0,0,0]$ | 한 특성에만 의존 | 패널티 큼 (비선호) |
| $w_2 = [0.25,0.25,0.25,0.25]$ | 모든 특성을 골고루 | 패널티 작음 (선호) |

정규화는 $w_{1}$처럼 가중치가 한쪽으로 치우친 경우에 패널티를 더 크게 주어서, 모든 특성을 골고루 보는 $w_{2}$를 선호하게 만든다.

> **정규화는 모델이 특정 특성에만 의존하지 않고 전반적인 패턴을 학습하도록 유도한다.** 이게 과적합 방지의 핵심 원리다.

---

## Softmax Classifier (Cross Entropy Loss)

### 점수를 확률로 — Softmax

Softmax는 점수를 확률로 바꿔주는 함수다:

$$P(Y=k|X=x_{i}) = \frac{e^{s_{k}}}{\sum\limits_{j}e^{s_{j}}}\ where\ s = f(x_{i};W)$$

### Cross Entropy Loss

그리고 Cross Entropy Loss는:

$$L_{i} = -log(\frac{e^{s_{y_{i}}}}{\sum\limits_{j}e^{s_{j}}})$$

이 수식이 처음에 제일 어려웠다. 특히 **왜 log를 쓰는지**가 납득이 안 됐는데, 양 극단을 따져보니 이해가 됐다.

| 정답 클래스 확률 | $-\log(\cdot)$ | loss |
|---|---|---|
| 1에 가까움 (정답을 확신) | $-log(1) = 0$ | 0 |
| 0에 가까움 (정답을 전혀 모름) | $-log(0) \rightarrow \infty$ | 폭발 |

즉 <mark>정답 클래스의 확률이 높을수록 loss가 작아진다.</mark> log가 들어간 이유는 정보이론에서 온 건데, 확률이 낮은 사건일수록 "놀라움(정보량)"이 크다는 직관과 연결된다.

### SVM vs Softmax — 무엇이 다른가

| 구분 | SVM Loss (Hinge) | Softmax (Cross Entropy) |
|---|---|---|
| 출력 해석 | 점수(margin) | 확률 |
| 학습 멈추는 시점 | 정답이 오답보다 마진만큼 높으면 멈춤 | 정답 확률을 계속 1로 밀어붙임 |
| 데이터 민감도 | 마진 넘으면 둔감 | 모든 데이터에 민감 |

> SVM은 정답이 오답보다 마진만큼 높으면 거기서 멈추지만, Softmax는 정답 확률을 계속 1에 가깝게 밀어붙인다. 모든 데이터에 대해 더 민감하게 반응한다.

---

## Optimization과 Gradient Descent

### gradient가 알려주는 것

Loss function으로 W가 얼마나 나쁜지를 알았으니, 이제 W를 개선해야 한다. weight가 변할 때 loss가 얼마나 변하는지, 즉 $\nabla WL = \frac{dW}{dL}$ 을 구하고 싶은 것이다.

먼저 numerical gradient로 "이 방향이 맞나?" 체크해보고, 확인되면 미분으로 analytic gradient를 구한다.

| 방식 | 특징 | 용도 |
|---|---|---|
| Numerical gradient | 근사적, 느림, 구현 쉬움 | 방향 검증(체크) |
| Analytic gradient | 정확, 빠름, 미분 필요 | 실제 학습 |

### vanilla Gradient Descent

```python
# vanilla Gradient Descent

while True:
    weights_grad = evaluate_gradient(loss_fun, data, weights)
    weights += - step_size * weights_grad # perform parameter update
```

$step\_size = learning\_rate = \alpha$

이게 결국 "경사를 따라 내려간다"는 뜻인데, learning rate가 너무 크면 loss가 발산(explode)하고, 너무 작으면 수렴이 안 된다.

---

## Mini-batch Gradient Descent

전체 데이터로 gradient를 계산하면 너무 느리니까, 일부분(mini-batch)으로 나눠서 계산한다.

| 질문 | 답 |
|---|---|
| 배치 크기는? | 컴퓨팅 환경에 맞게 (VRAM 크기에 맞게) |
| mini-batch 전체를 한 바퀴 돌면? | **1 epoch** |
| learning_rate가 너무 크면? | loss가 diverge, explode |
| mini-batch를 사용하는 대표 기법은? | **Stochastic GD** 등 |

---

## 전통적 이미지 분류 vs CNN

전통적 파이프라인은: 이미지에서 feature 추출 (HOG, SIFT, Bag of Words 등) → feature를 concat → linear classification 수행.

| 구분 | 전통적 파이프라인 | CNN |
|---|---|---|
| Feature 추출 | 수작업 (HOG, SIFT, BoW) | 학습으로 자동 추출 |
| 분류와의 관계 | 분리됨 | end-to-end 통합 |

> **CNN은 이 모든 과정을 하나의 모델로 end-to-end 학습한다.** Feature extraction과 classification을 분리하지 않는다는 게 핵심적인 차이다.

---

## 헷갈리기 쉬운 포인트

#### 1. "Cross Entropy에서 log는 왜 자연로그인가?"

밑이 2인 로그를 써도 Loss의 최소화 방향은 같다. 자연로그를 쓰는 이유는 미분이 깔끔하게 나오기 때문이다 ($\frac{d}{dx}ln(x) = \frac{1}{x}$). 수학적 편의성 문제지, 본질적 차이는 없다.

#### 2. "SVM Loss와 Cross Entropy 중 뭘 써야 하나?"

실무에서는 Cross Entropy(Softmax)를 훨씬 많이 쓴다. 확률값을 직접 얻을 수 있어서 모델의 "확신도"를 알 수 있기 때문이다. SVM Loss는 마진만 넘으면 더 이상 신경 쓰지 않아서 확률 해석이 안 된다.

#### 3. "정규화 강도 $\lambda$는 어떻게 정하나?"

이것도 하이퍼파라미터라서 validation set에서 실험으로 찾아야 한다. ~~크게 줄수록 일반화가 잘 되니까 무조건 크게~~ — 아니다. 너무 크면 모델이 아무것도 학습 못하고, 너무 작으면 과적합된다.

---

## 정리하며

이번 강의의 핵심은 한 줄로 정리된다.

> **Loss function으로 W의 품질을 측정하고, Gradient Descent로 W를 개선한다.**

다음 강의에서는 이 gradient를 효율적으로 계산하는 **역전파(Backpropagation)**를 배운다. 솔직히 수식이 많아서 부담됐는데, 하나하나 의미를 따져보니 결국 "정답 점수는 올리고, 오답 점수는 내린다"는 직관으로 수렴한다.
