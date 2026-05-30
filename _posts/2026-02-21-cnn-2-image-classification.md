---
layout: post
title: "CNN 2강 - 이미지 분류와 최근접 이웃 분류기"
description: "KNN이 이미지에서 왜 안 되는지, Linear Classification이 왜 필요한지 — 처음 배우면서 헷갈렸던 것들"
date: 2026-02-21
category: "Deep Learning"
subcategory: "CNN"
tags: deep-learning, cnn, image-classification, knn
comments: true
---

## 들어가며

CNN 스터디 2강에서는 이미지 분류의 기초를 배웠다. 솔직히 처음에는 "이미지를 분류한다"는 게 뭐가 그렇게 어려운 건지 감이 안 잡혔다. 사람 눈에는 고양이 사진이 고양이인 게 너무 당연한데, 컴퓨터한테는 그냥 숫자 배열일 뿐이라는 걸 이번에 제대로 체감했다.

처음엔 ~~그냥 비슷한 사진 찾으면 되는 거 아닌가~~ 싶었는데, 막상 들여다보니 "비슷하다"를 어떻게 정의하느냐부터가 문제였다. 이번 강의의 흐름은 결국 한 줄기로 이어진다.

> **데이터 기반 접근 → KNN → Linear Classification**
> — 단순한 "기억"에서 출발해 "파라미터 학습"으로 넘어가는 과정

---

## 이미지는 결국 숫자 배열이다

### Semantic Gap — 사람의 인식과 픽셀값의 간극

이미지는 3차원 배열로 표현된다. 예를 들어 300 $\cdot$ 100 픽셀의 컬러 이미지는 300 $\cdot$ 100 $\cdot$ 3 (RGB 채널)의 숫자 덩어리다.

여기서 <mark>semantic gap</mark>이라는 개념이 등장하는데, 사람이 "고양이"로 인식하는 것과 컴퓨터가 보는 픽셀값 사이에는 엄청난 간극이 있다는 뜻이다. 처음에는 "그냥 패턴 매칭하면 되는 거 아닌가?" 싶었는데, 아래 challenge들을 보고 생각이 바뀌었다.

### 이미지 분류를 어렵게 만드는 challenge들

| Challenge | 무엇이 문제인가 |
|---|---|
| **camera pose** | 같은 물체인데 찍는 각도가 다르면 완전 다른 이미지가 됨 |
| **illumination** | 조명만 바뀌어도 픽셀값이 확 달라짐 |
| **deformation** | 고양이가 웅크리면 서 있을 때와 완전 다른 모양 |
| **occlusion** | 물체가 가려져 있으면? |
| **background clutter** | 배경과 물체가 비슷한 색이면? |
| **intraclass variation** | 같은 "의자"인데 생김새가 천차만별 |

이걸 보면 하드코딩으로 이미지 분류기를 만드는 건 불가능하다는 결론이 나온다. 그래서 **데이터 기반 접근**이 필요하다.

```python
def train(train_images, train_labels):
    #build a model for images -> labels
    return model

def predict(model, test_images):
    #predict test_labels using the model
    return test_labels
```

모델을 학습한 후에 그 모델에 기반하여 예측한다. 이 패러다임이 딥러닝의 출발점이다.

---

## Nearest Neighbor Classifier

### 원리 — 가장 비슷한 이미지의 라벨 빌려오기

가장 단순한 데이터 기반 분류기가 NN(Nearest Neighbor)이다. CIFAR-10 데이터셋으로 학습하는데, 원리는 간단하다 — 테스트 이미지와 가장 비슷한 학습 이미지를 찾아서 그 라벨을 붙여주는 것이다.

"비슷하다"를 측정하기 위해 **L1 distance**를 사용한다:

$$L1\ distance,\ d_1(I_{1},I_{2}) = \sum\limits_{p}|I_{1}^{p}-I_{2}^{p}|$$

### 구현 코드

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

### "학습"이 아니라 "기억"이다

이 코드를 보면서 "어?" 했던 게, train 함수가 하는 일이 그냥 **데이터를 통째로 저장하는 것**밖에 없다.

> NN의 train은 학습이라기보다 기억이다. 그래서 예측할 때 모든 학습 데이터와 비교해야 하니까, 데이터가 늘어나면 분류 시간이 선형적으로 증가한다.

---

## L2 Distance와 K-Nearest Neighbor

### L2 (Euclidean) distance

L1 말고 **L2 (Euclidean) distance**도 쓸 수 있다:

$$Euclidean,\ d_{2}(I_{1}, I_{2}) = \sqrt{\sum\limits_{p}(I_{1}^{p}-I_{2}^{p})^{2}}$$

그리고 가장 가까운 1개가 아니라 **K개**의 이웃을 보고 다수결로 판단하는 게 K-Nearest Neighbor이다.

### 하이퍼파라미터를 정하는 법

여기서 중요한 게 <mark>하이퍼파라미터</mark> 개념이다. 어떤 distance를 쓸지, K를 몇으로 할지는 사람이 정해야 한다. 이걸 정하는 방법으로는:

| 방법 | 내용 |
|---|---|
| **validation set** | training data의 20%를 분리해서 실험 |
| **cross validation** | 데이터를 잘게 나눠서 교차 검증 수행 |

### 그런데 — KNN은 이미지에 안 쓴다

그런데 결론적으로, **KNN은 이미지 분류에서 절대 쓰지 않는다.** 이미지가 조금만 이동하거나, 가리거나, 어두워져도 L1/L2 distance는 "같은 물체"라고 판단하지 못하기 때문이다.

---

## Linear Classification

### Parametric Approach — 파라미터를 학습한다

그래서 등장하는 게 **Linear Classification**이다. Neural Network의 기본이 되는 <mark>parametric approach</mark>로, 파라미터(가중치)를 학습해서 분류한다:

$$f(x_{i}, W, b) = W\cdot x_{i}+b$$

### KNN과의 결정적 차이

| 항목 | KNN | Linear Classifier |
|---|---|---|
| 저장 대상 | 모든 학습 데이터 | 학습된 $W, b$ 만 |
| 예측 비용 | 데이터 수에 비례 (느림) | 행렬곱 한 번 (매우 빠름) |
| 학습 데이터 | 예측 때도 필요 | 학습 후 버려도 됨 |

정리하면, KNN은 모든 학습 데이터를 저장해야 하지만 Linear Classifier는 학습된 $W, b$ 만 있으면 되고, 예측 시 행렬곱 한 번이면 끝이라 **속도가 비교할 수 없이 빠르다**. 학습 데이터는 $W$ 를 학습하는 데만 쓰이고, 학습 후에는 버려도 된다.

### Linear Classification의 한계

다만 Linear Classification에도 한계가 있다.

- 회색 객체는 색 구분이 안 되어서 질감으로 분류해야 하는데 그게 어렵고
- 같은 객체인데 색이 다양하거나
- 같은 색인데 다른 객체인 경우도 분류하기 힘들다

이렇게 나온 예측값들이 정답과 얼마나 떨어져 있는지를 측정하는 **Loss function**, 그리고 그 Loss를 줄이기 위해 $W$ 를 조정하는 **Optimization**을 다음 강의에서 배운다.

---

## 헷갈리기 쉬운 포인트

이번 강의를 정리하면서 인터넷을 찾아보니 나만 헷갈린 게 아니었다. 많은 사람들이 비슷한 지점에서 막히더라.

#### 1. "KNN도 비선형 경계를 만들 수 있는데 왜 안 쓰나?"

KNN은 이론적으로 복잡한 결정 경계를 만들 수 있다. 하지만 이미지에서는 "비슷하다"의 기준이 픽셀값 차이인데, 이게 의미적 유사도와 전혀 다르기 때문이다. 고양이 사진을 1픽셀 오른쪽으로 밀면 L2 distance가 확 달라지지만, 사람 눈에는 같은 고양이다.

#### 2. "L1과 L2 중 뭘 써야 하나?"

정답은 없다. L1은 좌표축에 의존적이고(이미지를 45도 회전하면 결과가 달라짐), L2는 좌표축에 독립적이다. 실무에서는 보통 L2를 많이 쓰는데, 둘 다 해보고 validation에서 좋은 걸 고르는 게 맞다.

#### 3. "Linear Classifier가 뭘 학습한다는 건지 감이 안 잡힌다"

처음에 나도 이게 제일 어려웠다. 결국 $W$ 의 각 행이 각 클래스의 "템플릿"을 학습하는 것이다. $W$ 의 airplane 행을 이미지로 시각화하면 비행기 비슷한 형태가 나온다. 하지만 클래스당 템플릿이 1개뿐이라서, 다양한 형태의 물체를 하나의 템플릿으로 커버하는 데 한계가 있다.

---

## 정리하며

이미지 분류의 핵심 흐름은 한 줄기로 이어진다.

> **데이터 기반 접근 → KNN (단순하지만 느리고 이미지에 부적합) → Linear Classification (파라미터 학습, 빠르지만 한계 있음) → Loss function과 Optimization (다음 강의)**

결국 "어떻게 하면 $W$ 를 잘 학습할 수 있는가?"가 딥러닝의 핵심 질문이라는 걸 이번에 느꼈다.
