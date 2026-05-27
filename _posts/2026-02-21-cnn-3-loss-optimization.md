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

지난 강의에서 Linear Classifier로 $f(x, W) = Wx + b$ 를 배웠는데, 핵심 질문이 남았다 — "좋은 W는 어떻게 찾는가?" 이번 강의에서는 W가 얼마나 나쁜지를 측정하는 **Loss function**, 그리고 그걸 줄이는 **Optimization**을 배웠다.

솔직히 말하면, 수식이 여러 개 나오면서 처음에 좀 멘붕이었다. 특히 SVM Loss와 Softmax Loss가 각각 뭘 의미하는지, 왜 다른 함수를 쓰는지가 헷갈렸다. 정리하면서 겨우 감이 잡혔다.

---

## Multiclass SVM Loss (Hinge Loss)

주어진 예시 $(x_{i},y_{i})\ x_i = image,\ y_i = label$ 에 대해 SVM Loss는:

$$L_{i} = \sum\limits_{j \ne y_{i}}max(0, s_{j}-s_{y_{i}}+1)$$

- $s_j$ = 틀린 클래스 j의 예측 점수
- $s_{y_i}$ = 정답 클래스의 예측 점수

처음 이 수식을 봤을 때 "max(0, ...)은 뭐지?" 싶었는데, 의미를 풀어보면 간단하다. **정답 점수가 오답 점수보다 1 이상 높으면 loss가 0이고, 그렇지 않으면 그 차이만큼 벌점을 준다.** "1 이상 이기면 OK, 아니면 패널티" 같은 느낌이다.

가중치가 처음에 0에 가깝게 초기화되면 $s_j$와 $s_{y_i}$ 모두 0에 근사하게 되니까, max 함수에서 나오는 값들이 1에 근사하게 된다. 이런 식으로 초기 loss 값을 예측해보는 게 디버깅에 유용하다.

---

## 전체 손실 함수와 정규화(Regularization)

전체 데이터셋에 대한 손실 함수는:

$$L=\frac{1}{N}\sum\limits^{N}_{i=1}\sum\limits_{j \ne y_{i}}max(0, f(x_{i};W)_{j} - f(x_{i};W_{y_{i}})+1) + \lambda R(W)$$

여기서 $\lambda R(W)$가 **정규화(규제) 항**이다. 정규화의 종류:

- L1 regularization $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l}W^{2}_{k,l}$
- L2 regularization $\rightarrow$ $\sum\limits_{k}\sum\limits_{l}\|W_{k,l}\|$
- Elastic net(L1+L2) $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l} \beta W^{2}_{k,l}+\|W_{k,l}\|$
- Max norm regularization
- Dropout

처음에 "왜 정규화가 필요하지?"라고 생각했는데, 예시를 보니 이해가 됐다. $w_{1} = [1,0,0,0]$과 $w_{2}=[0.25,0.25,0.25,0.25]$가 있을 때, $x = [1,1,1,1]$에 대해 $w^{\top}_{1}x = w^{\top}_{2}x = 1$로 출력값이 같다. 하지만 정규화는 $w_{1}$처럼 가중치가 한쪽으로 치우친 경우에 패널티를 더 크게 주어서, 모든 특성을 골고루 보는 $w_{2}$를 선호하게 만든다.

즉, **정규화는 모델이 특정 특성에만 의존하지 않고 전반적인 패턴을 학습하도록 유도한다.** 이게 과적합 방지의 핵심 원리다.

---

## Softmax Classifier (Cross Entropy Loss)

Softmax는 점수를 확률로 바꿔주는 함수다:

$$P(Y=k|X=x_{i}) = \frac{e^{s_{k}}}{\sum\limits_{j}e^{s_{j}}}\ where\ s = f(x_{i};W)$$

그리고 Cross Entropy Loss는:

$$L_{i} = -log(\frac{e^{s_{y_{i}}}}{\sum\limits_{j}e^{s_{j}}})$$

이 수식이 처음에 제일 어려웠다. 특히 **왜 log를 쓰는지**가 납득이 안 됐는데, 이렇게 생각하니 이해가 됐다:

- Softmax가 내놓는 확률이 1에 가까우면 (정답을 확신) → $-log(1) = 0$ → loss가 0
- 확률이 0에 가까우면 (정답을 전혀 모름) → $-log(0) \rightarrow \infty$ → loss가 폭발

즉 **정답 클래스의 확률이 높을수록 loss가 작아진다.** log가 들어간 이유는 정보이론에서 온 건데, 확률이 낮은 사건일수록 "놀라움(정보량)"이 크다는 직관과 연결된다.

**SVM vs Softmax의 차이**: SVM은 정답이 오답보다 마진만큼 높으면 거기서 멈추지만, Softmax는 정답 확률을 계속 1에 가깝게 밀어붙인다. 모든 데이터에 대해 더 민감하게 반응한다.

---

## Optimization과 Gradient Descent

Loss function으로 W가 얼마나 나쁜지를 알았으니, 이제 W를 개선해야 한다. weight가 변할 때 loss가 얼마나 변하는지, 즉 $\nabla WL = \frac{dW}{dL}$ 을 구하고 싶은 것이다.

먼저 numerical gradient로 "이 방향이 맞나?" 체크해보고, 확인되면 미분으로 analytic gradient를 구한다.

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

- 배치 크기는? → 컴퓨팅 환경에 맞게 (VRAM 크기에 맞게)
- mini-batch 전체를 한 바퀴 돌면? → **1 epoch**
- learning_rate가 너무 크면? → loss가 diverge, explode
- **Stochastic GD** 등이 mini-batch를 사용한다

---

## 전통적 이미지 분류 vs CNN

전통적 파이프라인은: 이미지에서 feature 추출 (HOG, SIFT, Bag of Words 등) → feature를 concat → linear classification 수행.

하지만 **CNN은 이 모든 과정을 하나의 모델로 end-to-end 학습한다.** Feature extraction과 classification을 분리하지 않는다는 게 핵심적인 차이다.

---

## 헷갈리기 쉬운 포인트

**1. "Cross Entropy에서 log는 왜 자연로그인가?"**
밑이 2인 로그를 써도 Loss의 최소화 방향은 같다. 자연로그를 쓰는 이유는 미분이 깔끔하게 나오기 때문이다 ($\frac{d}{dx}ln(x) = \frac{1}{x}$). 수학적 편의성 문제지, 본질적 차이는 없다.

**2. "SVM Loss와 Cross Entropy 중 뭘 써야 하나?"**
실무에서는 Cross Entropy(Softmax)를 훨씬 많이 쓴다. 확률값을 직접 얻을 수 있어서 모델의 "확신도"를 알 수 있기 때문이다. SVM Loss는 마진만 넘으면 더 이상 신경 쓰지 않아서 확률 해석이 안 된다.

**3. "정규화 강도 $\lambda$는 어떻게 정하나?"**
이것도 하이퍼파라미터라서 validation set에서 실험으로 찾아야 한다. 너무 크면 모델이 아무것도 학습 못하고, 너무 작으면 과적합된다.

---

## 정리하며

이번 강의의 핵심은 **"Loss function으로 W의 품질을 측정하고, Gradient Descent로 W를 개선한다"**는 것이다. 다음 강의에서는 이 gradient를 효율적으로 계산하는 **역전파(Backpropagation)**를 배운다. 솔직히 수식이 많아서 부담됐는데, 하나하나 의미를 따져보니 결국 "정답 점수는 올리고, 오답 점수는 내린다"는 직관으로 수렴한다.
