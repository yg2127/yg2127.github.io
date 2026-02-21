---
layout: post
title: "CNN 3강 - 손실 함수와 최적화"
description: "SVM Loss, 정규화, Softmax, Cross Entropy, Gradient Descent"
date: 2026-02-21
category: "Deep Learning"
subcategory: "CNN"
tags: deep-learning, cnn, loss-function, optimization
comments: true
---

## 잘한 점

### 상황 1.

- **상황**: 다양한 손실 함수의 수학적 정의를 학습했다
- **액션**: SVM Loss와 Softmax Loss의 계산 과정을 단계별로 따라가며 정리했다
- **칭찬**: 수식을 단순히 외우지 않고 의미를 파악하려고 한 점이 좋았다

---

## 개선점

### 상황 1.

- **문제**: 정규화(Regularization) 종류별 차이가 헷갈린다
- **원인**: L1, L2, Dropout 등의 효과를 직접 실험해보지 않았다
- **액션플랜**: 각 정규화 방법을 코드로 구현하여 차이를 비교해보기

---

## 배운 점

### 상황 1. Multiclass SVM Loss (Hinge Loss)

- **배움**: 주어진 예시 $(x_{i},y_{i})\ x_i = image,\ y_i = label$ 에 대해 SVM Loss는 다음과 같이 정의된다.

$$L_{i} = \sum\limits_{j \ne y_{i}}max(0, s_{j}-s_{y_{i}}+1)$$

- $s_j$ = 객체 i에 대해 나온 출력 예측값에서 다른 라벨들의 값
- $s_{y_i}$ = 객체 i에 대해 나온 라벨 i에 대한 출력 예측값
- sum = 객체 i에 대해 i가 아닌 모든 라벨 j에 대해 각각 값을 구하고 모두 더한다는 의미

초기에 가중치가 0에 근접하게 초기화 될 텐데 그렇다면 loss 값은 어떻게 변하는가? $\rightarrow$ $s_j$, $s_{y_i}$ 값이 0에 근사하므로 max 함수를 통해 나온 값들은 1에 근사하게 된다.

- **의미**: SVM Loss의 구조를 이해하면 분류기가 정답 클래스와 오답 클래스의 점수 차이를 어떻게 벌리려 하는지 직관적으로 파악할 수 있다.

### 상황 2. 전체 손실 함수와 정규화(Regularization)

- **배움**: 전체 데이터셋에 대한 손실 함수는 다음과 같다.

$$L=\frac{1}{N}\sum\limits^{N}_{i=1}\sum\limits_{j \ne y_{i}}max(0, f(x_{i};W)_{j} - f(x_{i};W_{y_{i}})+1) + \lambda R(W)$$

$\lambda R(W)$는 가중치 규제 항이다. 정규화의 종류는 다음과 같다.

- L1 regularization $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l}W^{2}_{k,l}$
- L2 regularization $\rightarrow$ $\sum\limits_{k}\sum\limits_{l}\|W_{k,l}\|$
- Elastic net(L1+L2) $\rightarrow$ $R(W) = \sum\limits_{k}\sum\limits_{l} \beta W^{2}_{k,l}+\|W_{k,l}\|$
- Max norm regularization
- Dropout

예를 들어서 $w_{1} = [1,0,0,0]$, $w_{2}=[0.25,0.25,0.25,0.25]$ 의 경우 $x = [1,1,1,1]$ 일 때 출력 값 $w^{\top}_{1}x = w^{\top}_{2}x = 1$ 로 똑같은 출력 값이지만 regularization들은 $w_{1}$ 처럼 가중치가 한 쪽으로 치우친 경우 loss값을 더 크게 책정하므로 가중치의 모든 원소들을 균등하게 분포시키려 한다.

- **의미**: 정규화가 왜 필요한지, 그리고 가중치를 균등하게 분포시키는 원리를 이해하는 것이 과적합 방지의 핵심이다.

### 상황 3. Softmax Classifier (Cross Entropy Loss)

- **배움**: Softmax 분류기의 확률과 손실 함수는 다음과 같이 정의된다.

$$P(Y=k|X=x_{i}) = \frac{e^{s_{k}}}{\sum\limits_{j}e^{s_{j}}}\ where\ s = f(x_{i};W)$$

$$L_{i} = -log(\frac{e^{s_{y_{i}}}}{\sum\limits_{j}e^{s_{j}}})$$

로스 함수를 softmax 함수를 사용해서 0~1 사이의 값으로 loss값이 나온다.

Softmax vs SVM: softmax는 모든 데이터셋에 대해서 민감하게 반응한다.

- **의미**: Softmax는 확률 분포로 해석할 수 있어서 모델의 예측 신뢰도를 파악하는 데 유용하다.

### 상황 4. Optimization과 Gradient Descent

- **배움**: weight가 변할 때 loss가 얼마나 변하는지 $\nabla WL = \frac{dW}{dL}$ 을 구하고 싶은 것이다. gradient check를 하기 위해서 numerical gradient descent를 해보고 맞으면 이제 미분을 수행한다.

```python
# vanilla Gradient Descent

while True:
    weights_grad = evaluate_gradient(loss_fun, data, weights)
    weights += - step_size * weights_grad # perform parameter update
```

$step\_size = learning\_rate = \alpha$

- **의미**: Gradient Descent는 손실 함수를 최소화하기 위한 기본적인 최적화 방법이며, 학습률 설정이 핵심이다.

### 상황 5. Mini-batch Gradient Descent

- **배움**: 전체 데이터셋이 아닌 일부분(mini-batch)으로 gradient를 계산한다.
  - 데이터셋 샘플링하는 크기는? $\rightarrow$ 컴퓨팅 환경에 알맞게 (VRAM size에 맞게)
  - Mini-batch로 나눈 데이터셋 전체를 다 학습하면? $\rightarrow$ 1 epoch
  - learning_rate가 너무 크면? loss가 diverge, explode 하는 경우가 있을 수도 있다.
  - Stochastic GD 등이 mini-batch를 사용한다.

- **의미**: 실제 학습에서는 메모리 제약과 효율성 때문에 mini-batch를 사용하는 것이 필수적이다.

### 상황 6. Image Classification Pipeline (Old-School)

- **배움**: 전통적인 이미지 분류 파이프라인은 다음과 같다.
  1. image의 feature를 추출한다.
  2. 추출한 feature를 concat 한 후
  3. linear classification을 수행한다.

예시: HOG/SIFT features, Bag of Words 등등 다양한 방식으로 feature extraction 후 linear classification을 수행한다.

그러나 CNN은 이 모든 과정을 하나의 모델로 학습한다.

- **의미**: CNN이 기존 방식과 달리 feature extraction과 classification을 end-to-end로 학습한다는 점이 핵심적인 차이이다.
