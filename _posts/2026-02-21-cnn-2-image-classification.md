---
layout: post
title: "CNN 2강 - 이미지 분류와 최근접 이웃 분류기"
description: "이미지 표현, 분류 문제, Nearest Neighbor, KNN, Linear Classification"
date: 2026-02-21
category: "Deep Learning"
subcategory: "CNN"
tags: deep-learning, cnn, image-classification, knn
comments: true
---

## 잘한 점

### 상황 1.

- **상황**: CNN 팀스터디에서 이미지 분류의 기초 개념을 학습했다
- **액션**: L1, L2 거리 메트릭의 차이를 직접 비교하면서 정리했다
- **칭찬**: 수식뿐 아니라 직관적으로 이해하려고 노력한 점이 좋았다

---

## 개선점

### 상황 1.

- **문제**: KNN과 Linear Classification의 차이를 명확히 설명하기 어렵다
- **원인**: 각 알고리즘의 장단점을 비교 정리하지 않았다
- **액션플랜**: KNN vs Linear Classification 비교표를 만들어 정리하기

---

## 배운 점

### 상황 1. Pipeline과 이미지 표현

- **배움**: 이미지는 3차원 배열로 표현된다 (RGB처럼) 300 $\cdot$ 100 $\cdot$ 3 (300 x 100 픽셀에 한 픽셀마다 RGB값). 이미지 분류에는 semantic gap이 존재한다. 이미지 분류의 challenge는 다음과 같다:
  - camera pose (관점, 시각)
  - illumination (조명)
  - Deformation (변형)
  - Occlusion (은폐)
  - background clutter (배경과 유사함)
  - Intraclass variation (동일 객체의 다양성 문제)
- **의미**: 이미지 분류가 왜 어려운 문제인지, 어떤 요소들이 분류를 어렵게 만드는지 이해하는 것이 중요하다

### 상황 2. 데이터 기반 접근

- **배움**: 하드코딩 알고리즘으로는 이미지 분류가 불가능하다. 따라서 데이터에 기반한 접근이 필요하다. 많은 데이터를 학습한 후 분류하는 방식이다.

  ```python
  def train(train_images, train_labels):
      #build a model for images -> labels
      return model

  def predict(model, test_images):
      #predict test_labels using the model
      return test_labels
  ```

  모델을 학습한 후에 그 모델에 기반하여 예측한다.
- **의미**: 하드코딩이 아닌 데이터 기반 학습이라는 패러다임을 이해하는 것이 딥러닝의 출발점이다

### 상황 3. Nearest Neighbor Classifier

- **배움**: CIFAR-10 데이터셋으로 NN 모델을 학습한다. 이미지 비교를 위해 L1 distance를 사용한다.

  $$L1\ distance,\ d_1(I_{1},I_{2}) = \sum\limits_{p}|I_{1}^{p}-I_{2}^{p}|$$

  ```python
  import numpy as np

  class NearestNeighbor:
      def __init__(self):
          pass

      def train(self, X, y):
          # X is N times D where each row is an example. Y is 1-dimension of size N
          self.Xtr = X
          self.ytr = y

      def predict(self, X):
          # X is N times D where each row is an example we wish to predict label for
          num_test = X.shape[0]
          # lets make sure that the output type matches the input type
          Ypred = np.zeros(num_test, dtype = self.ytr.dtype)

          # loop over all test rows
          for i in xrange(num_test):
              # find the nearest training image to the i'th test image
              # using the L1 distance (sum of absolute value difference)
              distances = np.sum(np.abs(self.Xtr - X[i,:]), axis = 1)
              min_index = np.argmin(distances)
              # get the index with smallest distance
              Ypred[i] = self.ytr[min_index]
              # predict the label of the nearest example

          return Ypred
  ```

  train에서는 모든 train 데이터를 메모리 상에 올려서 기억하게 한다. 예측에서는 for문 안에서 테스트 데이터셋 이미지를 입력받고 distance = L1, min_index는 거리가 가장 작은 train set 중 이미지를 찾게 되는 것이다. 데이터셋 크기가 늘어날수록 분류 시간은 선형적으로 증가하게 된다. 메모리에 올라가는 데이터셋 양이 그만큼 많아지게 되기 때문이다.
- **의미**: Nearest Neighbor의 동작 원리와 한계(선형적 시간 증가)를 이해하면, 더 효율적인 분류기가 왜 필요한지 알 수 있다

### 상황 4. L2 Distance와 K-Nearest Neighbor

- **배움**: L1 말고 L2 (Euclidean) distance도 사용할 수 있다.

  $$Euclidean,\ d_{2}(I_{1}, I_{2}) = \sqrt{\sum\limits_{p}(I_{1}^{p}-I_{2}^{p})^{2}}$$

  K-Nearest Neighbor는 K개의 이미지와 가장 가까운 이미지를 분류하는 기법이다. 어떤 distance를 사용하느냐, 어떤 K를 사용하느냐 같은 하이퍼파라미터 설정이 중요하다. training data의 20%를 validation으로 사용하여 하이퍼파라미터 조정에 사용한다. 혹은 train set을 잘게 나눠서 cross validation을 할 수도 있다. K-NN은 이미지 상에서 절대 사용하지 않는다. 이동하거나 가리거나 조금만 어둡게 해도 같게 판단하기 때문이다.
- **의미**: 하이퍼파라미터 튜닝 방법(validation set, cross validation)과 KNN의 한계를 아는 것이 모델 선택의 기초가 된다

### 상황 5. Linear Classification

- **배움**: Neural Network의 기본이 되는 Linear Classification은 parametric approach이다. 파라미터 조정에 따른 문제 접근 방식이다.

  $$f(x_{i}, W, b) = W\cdot x_{i}+b$$

  Linear classification으로 분류하기 힘든 것들이 있다. 회색으로 된 객체는 색 구분이 안되고 텍스처, 질감으로 분류해야 하므로 분류가 힘들다. 같은 객체이나 다양한 색을 가진 객체들도 분류하기 힘들다. 혹은 같은 색이지만 다른 객체들도 분류하기 힘들다.

  이렇게 나온 값들이 정답과 얼마나 떨어져 있는지, 얼마나 가까운지에 대해 판단하는 함수 Loss function을 정의하고 그 Loss function으로 가중치 W를 조정하는 optimizing을 배울 것이다.
- **의미**: Linear Classification의 수식과 한계를 이해하면, 이후 배울 Loss function과 Optimization의 필요성을 자연스럽게 이해할 수 있다
