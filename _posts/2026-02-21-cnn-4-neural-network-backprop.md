---
layout: post
title: "CNN 4강 - 신경망과 역전파"
description: "Gradient Descent, Neural Network, 활성화 함수, Backpropagation, Jacobian"
date: 2026-02-21
category: "Deep Learning"
subcategory: "CNN"
tags: deep-learning, cnn, neural-network, backpropagation
comments: true
---

## 잘한 점

### 상황 1.
- **상황**: 역전파(Backpropagation)의 수학적 원리를 학습했다
- **액션**: Chain Rule을 따라가면서 각 레이어의 그래디언트 흐름을 직접 추적했다
- **칭찬**: 추상적인 개념을 구체적인 계산 과정으로 이해하려 한 점이 좋았다

---

## 개선점

### 상황 1.
- **문제**: 활성화 함수별 특성 차이를 직관적으로 설명하기 어렵다
- **원인**: ReLU, Sigmoid, Tanh 등의 그래프와 미분값을 비교 정리하지 않았다
- **액션플랜**: 각 활성화 함수의 그래프와 gradient 특성을 시각화하여 비교하기

---

## 배운 점

### 상황 1. Gradient Descent
- **배움**: 가장 예측을 잘하는 W를 구하기 위해 경사 하강법을 사용한다. 스텝 크기(learning_rate)에 따라 가중치가 얼마나 많이 업데이트 될 지에 대해 결정한다. Mini-batch를 통해 전체 데이터셋에서 나눠서 스텝으로 업데이트한다. learning_rate_scheduling을 통해 스텝 크기를 조절해서 Loss를 낮출 수 있다.
- **의미**: 경사 하강법의 핵심 하이퍼파라미터인 learning rate와 mini-batch의 역할을 정리해두면, 모델 학습 시 튜닝 방향을 잡는 데 도움이 되기 때문이다.

### 상황 2. Neural Network 구조
- **배움**: 신경망은 다음과 같이 표현된다.

$$f = Wx,\ x \in \mathbb{R}^{D},\ W_{1} \in \mathbb{R}^{H \times D},\ W_{2}\in \mathbb{R}^{C \times H}$$

$$f_{2} = W_{2}max(0,W_{1}x)$$

$$f_{3} = W_{3}max(0, f_{2})$$

활성화 함수 max(0,x)는 ReLU 함수이며 비선형성을 보장한다. **비선형성 보장을 통해 모델을 심층으로 만들 수 있고 더 복잡하고 세밀한 분류/추론이 가능해지기 때문이다.** 입력값 x와 출력값 f의 벡터 크기는 가중치의 벡터 사이즈로 결정된다.
- **의미**: 신경망의 수학적 표현과 비선형 활성화 함수의 역할을 이해하는 것이 딥러닝 아키텍처의 기초이기 때문이다.

### 상황 3. 활성화 함수 종류
- **배움**: 대표적인 활성화 함수는 다음과 같다.
  - ReLU
  - Leaky ReLU
  - Sigmoid
  - Tanh
  - ELU
  - GELU (트랜스포머 신경망 아키텍쳐에서 많이 사용된다.)

sigmoid, Tanh는 출력값이 -1 ~ 1 까지이며 기울기값도 0~1 사이이므로 Gradient Vanishing이 일어날 수 있다.
- **의미**: 활성화 함수 선택이 모델의 학습 안정성과 성능에 직접적인 영향을 미치므로, 각 함수의 특성을 알아두는 것이 중요하기 때문이다.

### 상황 4. 규제항과 뉴런 개수
- **배움**: 규제항(L1, L2 regularization)으로 일반화 성능을 높이도록 사용한다. ($\lambda$) 문제 해결에 있어서 필요한 뉴런의 개수는 실증적인 실험결과와 유사한 사례를 찾아보고, 실험을 통해서 알아본다.
- **의미**: 과적합 방지를 위한 규제 기법과 네트워크 크기 결정 방법을 정리해두면 실제 모델 설계 시 참고할 수 있기 때문이다.

### 상황 5. Back Propagation과 Chain Rule
- **배움**: 전체 네트워크에 대해 Loss값을 편미분한 것은 다음과 같다.

$$\frac{\partial L}{\partial f}$$

Loss에 대한 가중치 값의 편미분을 나타내기 위해 Loss 함수를 f에 대해 편미분하고 f를 W에 대해 편미분한 값으로 나타낼 수 있다.

$$\frac{\partial L}{\partial W} = \frac{\partial f}{\partial W} \cdot \frac{\partial L}{\partial f}$$

$f(W,X) = \frac{1}{1+e^{-(W \cdot X)}}$ 일 때 또한 손실함수가 softmax Loss일 때

$$L_{i} = -log\left(\frac{e^{f}}{\sum\limits_{all}e^{f}}\right)$$

위의 각각의 항 $\frac{\partial f}{\partial W}$, $\frac{\partial L}{\partial f}$는 각각의 수식을 편미분해서 구할 수 있다. 위 계산을 심플하게 하기 위해 파이토치 라이브러리는 sigmoid, ReLU 같은 함수를 다 미분하고 계산해주는 클래스를 갖고 있다.
- **의미**: 역전파의 핵심인 Chain Rule 기반 그래디언트 계산 과정을 수식으로 이해하면, 커스텀 레이어나 손실 함수를 구현할 때 디버깅이 수월해지기 때문이다.

### 상황 6. Jacobian 행렬
- **배움**: 입력 벡터 x가 신경망 f를 통과해서 y로 출력될 때 vector to vector 행렬 변환이 일어난다고 할 수 있다. $x \in \mathbb{R}^{N},\ y \in \mathbb{R}^{M}$ 일 때 야코비안 행렬식 $\frac{\partial y}{\partial x} \in \mathbb{R}^{N \times M}$을 통해서 입력 벡터 x값이 미소 단위로 변할 때 출력 벡터 y가 극소적으로 얼마나 변하게 되는지 알 수 있다. 그런데 $N \times M$의 큰 연산을 해서 야코비안 행렬식을 구하고 이걸 각 함수마다 계산하는 건 매우 크니까 행렬 변환식인 $W_{N \times M}$의 편미분 값을 계산하여 단순한 미분 계산으로 쉽게 계산할 수 있다.
- **의미**: Jacobian의 개념과 이를 효율적으로 계산하는 방법을 이해하면, 대규모 신경망에서의 그래디언트 계산이 왜 실용적으로 가능한지 이해할 수 있기 때문이다.
