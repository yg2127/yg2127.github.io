---
layout: post
title: "FFDP 수식과 학습 구조 - 좌표가 복원되는 과정을 따라가기"
description: "손으로 계산한 DWT에서 시작해 FFDP의 계수 조절, 검출 좌표 중심 확산, DDIM 갱신, 두 단계 학습과 gradient 경로를 하나씩 따라간다."
date: 2026-09-08
permalink: /papers/ffdp/math-and-training/
category: "Paper Review"
subcategory: "Pose Estimation / Diffusion"
tags: [deep-learning, ffdp, paper-review, pose-estimation, diffusion, wavelet, dwt, ddim]
comments: true
paper_review: true
series: ffdp
series_title: "FFDP를 이해한 과정"
series_order: 2
---

## 들어가며

[1편](/papers/ffdp/)에서는 FFDP가 가려진 관절을 왜 주파수 영역에서 복원하는지 살펴봤다. 이제 블록 이름을 실제 계산으로 바꿔 볼 차례다.

손필기에는 한 관절의 좌표를 두 개씩 묶어 평균과 차이를 계산한 흔적이 있다. 이어서 프레임과 관절 좌표를 행렬로 적고, DWT 식의 $m,n,k_1,k_2$가 어디를 가리키는지 하나씩 추적했다. 마지막에 남은 질문은 더 구체적이었다.

> “인코더가 정확하게 어떤 입력, 어떤 출력을 해서 어떻게 학습을 하게 되는건지가 중요한 거 아냐?”
>
> “인코더, 디코더의 입출력과 역전파 시 어디가 학습되는지, 어느 부분의 가중치가 업데이트 되는지 다 알려줘.”

이 글은 그 질문에 답하기 위해 좌표 하나에서 출발한다. DWT로 네 부대역을 만들고, 학습된 계수 맵으로 조절한 뒤, 검출 좌표 중심의 확산과 DDIM을 거쳐 2D·3D 동작이 학습되는 경로를 따라간다.

> **읽는 기준**<br>
> 논문 본문과 보충자료로 확인한 사실, 계산을 이해하기 위해 이번에 만든 예시를 구분한다. FFDP 공식 코드 공개 주소는 확인 시점에 접근할 수 없었다. 따라서 wavelet family, 경계 처리, 관절 수와 배열 순서, coefficient map의 활성함수, 정확한 DDIM schedule은 구현 사실로 단정하지 않는다.

---

## 1. 한 관절의 좌표를 평균과 차이로 나눈다

손필기의 첫 계산은 다음 시계열이었다.

**[설명용 유도] 손필기에 남은 평균·반차 방식의 Haar 예시**

$$
z=[10,12,14,30,31,32,33,34].
$$

인접한 값을 두 개씩 묶고 평균과 반차를 계산한다.

**[설명용 유도]**

$$
L_1[k]=\frac{z[2k]+z[2k+1]}{2},
\qquad
H_1[k]=\frac{z[2k]-z[2k+1]}{2}.
$$

그러면 첫 level의 결과는 다음과 같다.

**[설명용 유도]**

$$
L_1=[11,22,31.5,33.5],
\qquad
H_1=[-1,-8,-0.5,-0.5].
$$

$H_1$의 두 번째 값이 $-8$인 이유는 $14$와 $30$의 차이가 크기 때문이다.

**[설명용 유도]**

$$
H_1[1]=\frac{14-30}{2}=-8.
$$

이 값은 3번째와 4번째 위치 사이에 큰 변화가 있다는 사실을 드러낸다. 하지만 여기서 바로 “$-8$은 잡음이다”라고 말할 수는 없다. 사람이 실제로 팔을 빠르게 움직여도 큰 고주파 계수가 생기고, 가려진 관절의 검출 좌표가 튀어도 같은 현상이 생긴다.

DWT가 해 주는 일은 변화의 위치와 크기를 다른 성분으로 펼쳐 보이는 데까지다. 그 변화가 보존할 동작인지 억제할 오류인지는 뒤의 학습 모델이 주변 프레임과 관절의 관계를 보고 판단해야 한다.

### 더 깊은 level은 더 넓은 구간을 본다

$L_1$에 같은 연산을 한 번 더 적용하면 다음 결과를 얻는다.

**[설명용 유도]**

$$
L_2=[16.5,32.5],
\qquad
H_2=[-5.5,-1].
$$

$H_1$의 원소 하나는 원래 좌표 두 개의 차이를 요약한다. $H_2$의 원소 하나는 첫 level에서 두 쌍을 평균 낸 값들의 차이이므로 원래 좌표 네 개 범위에 대응한다. 손필기에서 이를 “마치 심층 convolution 연산처럼”이라고 적은 이유도 각 level에서 보는 시간 범위가 넓어지기 때문이다.

다만 이것은 다중 level을 이해하기 위한 예시다. **FFDP 보충자료가 밝힌 실제 decomposition level은 1**이다. 또한 위 계산은 $1/2$를 쓰는 평균·반차 방식이다. 정규직교 Haar에서 자주 쓰는 $1/\sqrt2$ 정규화와 같지 않으며, FFDP가 어떤 wavelet family와 정규화를 썼는지는 공개 문헌에 명시되지 않았다.

---

## 2. 한 줄 좌표가 프레임과 관절의 행렬이 된다

한 관절의 한 좌표만 보면 입력은 $z$라는 벡터다. 실제 모델은 여러 프레임의 여러 관절을 함께 본다.

**[논문 원문] 2D 키포인트 시퀀스의 형태**

$$
P_t\in\mathbb{R}^{N\times2J}.
$$

- $N$: 한 입력 window의 프레임 수
- $J$: 사용하는 관절 수
- $2J$: 각 관절의 $x,y$ 좌표를 펼친 열의 수
- $t$: diffusion의 noise level

**[보충자료]** FFDP의 temporal window는 36프레임이다. 따라서 한 번의 모델 호출은 36프레임 전체의 좌표 배열을 받는다. 손필기에는 $36\times28$, 즉 14관절을 가정한 크기가 적혀 있었지만, FFDP 본문과 보충자료만으로 실제 $J=14$와 열 순서를 확정할 수는 없다. OcMotion과 연결된 다른 저장소에서 14관절을 썼다는 사실도 FFDP 모델의 입력 크기를 대신 증명하지 않는다.

### $m,n$이 움직이는 두 축

논문의 DWT 식은 두 축을 한꺼번에 처리한다.

**[논문 원문] 식 (3)**

$$
y_{h,v}[k_1,k_2]
=\sum_m\sum_n
P[m,n]f_h[m-2k_1]f_v[n-2k_2],
\qquad h,v\in\{L,H\}.
$$

이 글에서는 다음과 같이 고정해서 읽는다.

| 기호 | 이 글에서의 의미 |
|---|---|
| $m$ | 입력의 프레임축 인덱스 |
| $n$ | 펼친 관절·좌표축 인덱스 |
| $k_1$ | 시간축 필터링 뒤 출력 위치 |
| $k_2$ | 관절·좌표축 필터링 뒤 출력 위치 |
| $f_L$ | low-pass filter |
| $f_H$ | high-pass filter |

손필기 3쪽에서는 $m$을 시간축, $n$을 관절·좌표축으로 적었지만 5쪽에서는 둘을 반대로 적었다. 계산의 핵심은 기호 이름 자체가 아니라 **각 필터가 어느 축을 따라 이동하는가**다. 그래서 여기서는 논문 식의 첫 합을 시간축, 두 번째 합을 펼친 좌표축으로 일관되게 사용한다.

### stride 2가 입력을 두 칸씩 묶는다

1차원 low-pass 식부터 보면 인덱스가 선명해진다.

**[설명용 유도] 손필기의 1차원 식을 정리한 형태**

$$
y[k]=\sum_m z[m]f_L[m-2k].
$$

$f_L[0]$과 $f_L[1]$에서만 값이 있고 나머지는 0이라고 하자. 그러면 $k=0$일 때는 $z[0],z[1]$, $k=1$일 때는 $z[2],z[3]$만 남는다.

**[설명용 유도]**

$$
\begin{aligned}
y[0]&=z[0]f_L[0]+z[1]f_L[1],\\
y[1]&=z[2]f_L[0]+z[3]f_L[1],\\
y[2]&=z[4]f_L[0]+z[5]f_L[1].
\end{aligned}
$$

설명용 필터에서 support 밖의 값은 0이므로 해당 항은 계산에서 제외된다. 복잡해 보이는 $m-2k$는 결국 필터를 두 칸씩 옮기는 인덱스다.

[![DWT가 시간축과 펼친 관절 좌표축을 따라 이동하는 작은 행렬 예시](/assets/ffdp/explainer-dwt-grid.svg)](/assets/ffdp/explainer-dwt-grid.svg)
*[설명용 유도] 작은 행렬에 평균·반차 필터를 적용해 LL·LH·HL·HH가 만들어지는 위치를 나타낸 그림이다. FFDP의 미명시 wavelet 설정을 재현한 그림은 아니다.*

그림의 입력과 결과는 다음과 같다. 첫 글자는 시간축, 두 번째 글자는 펼친 좌표 열축의 filter다.

**[설명용 유도] 실행으로 검증한 계산**

$$
P=
\begin{bmatrix}
10&12&20&22\\14&16&24&26\\18&20&28&30\\30&32&40&50
\end{bmatrix},
$$

$$
LL=\begin{bmatrix}13&23\\25&37\end{bmatrix},\quad
LH=\begin{bmatrix}-1&-1\\-1&-3\end{bmatrix},
$$

$$
HL=\begin{bmatrix}-2&-2\\-6&-8\end{bmatrix},\quad
HH=\begin{bmatrix}0&0\\0&2\end{bmatrix}.
$$

같은 평균·반차 규칙으로 iDWT를 적용하면 원래 $P$가 정확히 복원된다. 계산은 [검증 스크립트](/assets/code/ffdp-study/verify_examples.py)에서 다시 실행할 수 있다.

2차원에서는 시간축과 관절·좌표축에 low/high filter를 조합해 네 부대역을 만든다.

| 부대역 | 시간축 | 펼친 관절·좌표축 | 읽을 수 있는 성질 |
|---|---|---|---|
| LL | Low | Low | 두 축에서 완만한 변화 |
| LH | Low | High | 시간에는 완만하고 인접 열 사이 차이는 큰 변화 |
| HL | High | Low | 시간에는 빠르고 인접 열에서는 비슷한 변화 |
| HH | High | High | 두 축 모두에서 빠른 변화 |

여기서 “LH는 팔을 든 동작”, “HH는 떨림”처럼 동작 이름을 고정하면 안 된다. 부대역은 필터 응답의 종류다. 실제 움직임의 의미는 관절 배열, 주변 프레임, 모델이 학습한 관계까지 함께 봐야 정해진다.

펼친 관절·좌표축에는 더 근본적인 질문도 남는다. 배열에서 이웃한 열이 골격에서 이웃한 관절이라는 보장은 없다. $x,y$가 어떤 순서로 배치되는지에 따라 high-pass 응답도 달라진다. 정확한 joint ordering이 공개되지 않은 현재에는 “공간축 DWT가 골격의 국소 이웃을 직접 본다”고 단정할 수 없다.

---

## 3. 계수를 만든 뒤 모델이 배율을 학습한다

DWT는 고정 변환이다. 같은 입력과 같은 filter를 넣으면 같은 부대역을 만든다. DWT 자체가 데이터에서 “이 값은 잡음”이라는 규칙을 배우지는 않는다.

FFDP의 prior network는 네 부대역을 받아 refined coefficient와 coefficient map을 함께 출력한다.

**[논문 원문] 식 (4)**

$$
\hat y_{h,v},m_{h,v}=F_{h,v}(y).
$$

그리고 둘을 원소별로 곱한다.

**[논문 원문] coefficient selection**

$$
\bar y_{h,v}=m_{h,v}\odot\hat y_{h,v}.
$$

$\odot$는 같은 위치의 값을 곱하는 Hadamard product다. 예를 들어 한 위치에서 refined coefficient가 $-8$이고 map 값이 $0.2$라면 출력은 $-1.6$이 된다.

**[설명용 유도]**

$$
(-8)\times0.2=-1.6.
$$

이 숫자는 곱의 역할만 보여 주는 예시다. 논문은 $m$의 범위나 활성함수를 밝히지 않았으므로 실제 값이 $0$과 $1$ 사이인지도 확인할 수 없다.

논문이 이 과정을 coefficient **selection**이라고 부르지만, top-k나 binary mask를 사용한다고 명시하지는 않는다. 확인 가능한 정의는 각 위치에서 예측한 값들을 원소별로 곱하는 연산이다. 모든 고주파를 제거하는 fixed cutoff로 해석해서도 안 된다.

필터링한 네 부대역은 iDWT로 다시 좌표 배열이 된다.

**[설명용 유도] 논문 식 (5)를 압축한 표기**

$$
P=\operatorname{iDWT}(\bar y_{LL},\bar y_{HL},\bar y_{LH},\bar y_{HH}).
$$

이 간단한 표기는 논문 식 (5)의 합을 역할 중심으로 줄여 쓴 것이다. 모델은 좌표를 직접 한 번에 회귀하는 대신, 주파수 위치별 계수를 조절한 뒤 좌표 영역으로 돌아온다.

---

## 4. 가우시안은 좌표를 고치는 장치가 아니라 출발점이다

손필기 다음 부분에는 “가림 영역의 키포인트를 가우시안으로 억제한다는 의미”라는 질문이 있었다. 여기에는 분포와 denoiser의 역할이 섞여 있다.

가우시안은 가림 잡음을 억제하지 않는다. 신뢰하기 어려운 검출 좌표의 불확실성을 표현하고 초기 좌표 표본을 만드는 역할을 한다. 실제 복원은 학습된 network가 담당한다.

한 관절의 검출 좌표가 다음과 같다고 하자.

**[설명용 유도] 손필기의 좌표 예시**

$$
p=(100,200).
$$

논문은 불신뢰 관절에 대해 이 좌표를 평균으로 하고 단위행렬을 공분산으로 하는 분포를 둔다.

**[논문 원문] 불신뢰 관절의 초기분포**

$$
\mathcal N(p,I).
$$

여기서 모델에 들어가는 것은 “가우시안”이라는 수학 객체 자체가 아니다. 분포에서 뽑은 구체적인 2차원 좌표다.

**[설명용 유도] $\mathcal N(p,I)$에서 좌표 표본을 만드는 정의**

$$
\epsilon\sim\mathcal N(0,I),
\qquad
p_T=p+\epsilon.
$$

같은 $p$에서도 표본은 매번 달라질 수 있다. 하지만 random sample과 학습 가능한 복원 함수는 충돌하지 않는다. 같은 정답을 여러 잡음 상태로 보여 주면 network는 주변 관절과 프레임을 이용해 깨끗한 좌표를 되찾는 공통 규칙을 학습할 수 있다.

confidence는 어느 관절을 믿을지 나누는 기준이다. **[보충자료]** FFDP는 ViTPose++-B와 threshold 0.7을 사용한다. 다만 0.7은 보충자료에서 선택한 설정이지 모든 detector와 dataset에 보편적인 임계값은 아니다. confidence가 관절별 가우시안 분산으로 직접 바뀐다는 설명도 논문에는 없다.

---

## 5. forward 식은 정답에서 검출 좌표 쪽으로 이동한다

일반 diffusion은 깨끗한 데이터를 표준정규분포 쪽으로 흐리게 만드는 설명으로 자주 배운다. FFDP는 detector 좌표라는 중심을 활용한다.

**[논문 원문] 식 (1)**

$$
q(p_t\mid\hat p_0)
=p+\sqrt{\hat\alpha_t}(\hat p_0-p)
+\sqrt{1-\hat\alpha_t}\epsilon,
\qquad
\epsilon\sim\mathcal N(0,I).
$$

**[논문 원문] 누적 계수**

$$
\hat\alpha_t=\prod_{i=0}^{t}\alpha_i.
$$

기호를 좌표의 종류로 읽어 보자.

| 기호 | 의미 |
|---|---|
| $p$ | 2D detector가 낸 관절 좌표 |
| $\hat p_0$ | 학습 때 사용하는 깨끗한 정답 좌표 |
| $p_t$ | noise level $t$의 noisy 좌표 표본 |
| $\epsilon$ | 표준정규분포에서 뽑은 잡음 |
| $\hat\alpha_t$ | 처음부터 $t$까지 곱한 누적 계수 |

누적 계수가 1에 가까우면 정답 좌표의 영향이 크다. 0에 가까워질수록 중심이 detector 좌표 $p$ 쪽으로 이동하고 잡음 비중이 커진다.

**[설명용 유도] 식 (1)의 조건부 평균**

$$
\mathbb E[p_t\mid\hat p_0,p]
=(1-\sqrt{\hat\alpha_t})p
+\sqrt{\hat\alpha_t}\hat p_0.
$$

$p_T=p+\epsilon$은 $\mathcal N(p,I)$에서 표본을 만드는 정의다. 이와 달리 forward 식 (1)의 유한한 마지막 시점이 그 표본과 같아지는지는 $\hat\alpha_T$에 달려 있다. $\hat\alpha_T$가 충분히 작으면 식 (1)이 detector 중심 가우시안에 **가까워진다**고 설명할 수 있지만, 공개된 schedule에서 정확히 0인지는 확인되지 않았다. 두 식은 학습과 추론에 서로 다른 forward 공식을 쓴다는 뜻이 아니다.

또한 이 확산은 불신뢰 관절에 적용된다. 논문은 reliable keypoint가 forward와 reverse diffusion에서 원래 관측값을 유지한다고 명시한다.

---

## 6. 36프레임과 DDIM 5 step은 다른 축이다

대화에서 가장 오래 걸린 지점은 두 종류의 시간이 같은 문자 주변에 나타난다는 점이었다.

> “5번 반복한다고 했는데 어떻게 5번 반복해서 36프레임이 나오는거야?”

36은 영상 프레임 축의 길이다. DDIM 5 step은 같은 36프레임 좌표 배열을 서로 다른 noise level에서 다섯 번 정제하는 계산 축이다.

[![36프레임 영상축과 DDIM의 noise-level 축을 분리한 그림](/assets/ffdp/explainer-diffusion-axes.svg)](/assets/ffdp/explainer-diffusion-axes.svg)
*36프레임 전체가 각 sampling step을 통과한다. 다섯 번의 모델 호출이 다섯 프레임을 생성하는 구조가 아니다.*

**[보충자료]** 학습 diffusion timestep은 100이고, 추론은 DDIM 5 step을 사용한다. 이것은 다음 뜻이다.

```text
한 모델 평가의 입력: 36프레임 전체의 noisy keypoint sequence
                     ↓
다음 noise level:    같은 36프레임 전체의 다음 상태 sequence
                     ↓
최종 출력:           같은 window에 대한 복원 결과
```

손필기에는 100개 중 “무작위 5개”를 고른다는 설명도 있었다. 하지만 보충자료는 5-step DDIM이라고만 밝힌다. 매 추론마다 무작위 시점을 고르는지, 어떤 index를 선택하는지, noise schedule과 DDIM의 $\eta$가 무엇인지는 명시하지 않았다.

`100,75,50,25,0`처럼 상태 다섯 개를 나열하면 상태 사이 이동은 네 번이라는 점도 조심해야 한다. 이 숫자를 FFDP의 정확한 다섯 번 모델 호출 schedule로 사용할 수 없다.

---

## 7. 현재 입력과 깨끗한 좌표 예측을 DDIM 식에 넣는다

논문은 reverse distribution을 다음처럼 쓴다.

**[논문 원문] 식 (2)**

$$
q(P_{t-1}\mid P_t)
=\mathcal N\!\left(P_{t-1};\mu_\alpha(P_t),\tilde\gamma_tI\right).
$$

이어 모델이 매 timestep마다 $P_0$를 예측하고, 식 (1)을 이용해 다음 상태를 구성한다고 설명한다. 그러나 $\mu_\alpha$의 닫힌식과 DDIM update를 본문에 풀어 쓰지는 않는다.

그래서 다음 계산은 FFDP의 공개 구현을 옮긴 것이 아니다. FFDP 식 (1)을 detector-centered residual 좌표로 바꾸고, 원 DDIM의 deterministic update를 적용해 현재 입력과 깨끗한 예측의 역할을 보여 주는 유도다.

먼저 detector 좌표 $p$를 원점으로 옮긴다.

**[설명용 유도]**

$$
z_t=P_t-p,
\qquad
z_0=P_0-p.
$$

그러면 forward 식은 표준 diffusion 형태가 된다.

**[설명용 유도]**

$$
z_t=\sqrt{\bar\alpha_t}z_0
+\sqrt{1-\bar\alpha_t}\epsilon.
$$

현재 noisy input $P_t$와 모델이 방금 예측한 clean pose $P_0^{\mathrm{pred},(t)}$를 이용하면, 둘과 일관된 예측 잡음을 역산할 수 있다.

**[설명용 유도]**

$$
\epsilon_t^{\mathrm{pred}}
=\frac{
P_t-p-\sqrt{\bar\alpha_t}
\left(P_0^{\mathrm{pred},(t)}-p\right)
}{\sqrt{1-\bar\alpha_t}}.
$$

여기서 $P_t$는 현재 상태에, $P_0^{\mathrm{pred},(t)}$는 clean coordinate 항에 들어간다. 이전 DDIM 결과를 식의 정답 자리에 단순 대입하는 계산이 아니다.

더 이른 noise level $s<t$로 이동할 때 deterministic DDIM, 즉 $\eta=0$인 설명용 갱신은 다음과 같다.

**[설명용 유도: DDIM 원 논문의 $\eta=0$ 구조를 FFDP residual 좌표에 적용]**

$$
P_s
=p+\sqrt{\bar\alpha_s}
\left(P_0^{\mathrm{pred},(t)}-p\right)
+\sqrt{1-\bar\alpha_s}\epsilon_t^{\mathrm{pred}}.
$$

$\bar\alpha_t$는 현재 noise level, $\bar\alpha_s$는 이동할 noise level의 누적 계수다. 모델을 다시 호출하면 새로운 $P_s$에서 clean pose를 다시 예측하므로 $\epsilon^{\mathrm{pred}}$도 매 step 새로 계산된다.

작은 숫자로 대입하면 각 항이 더 잘 보인다. 검출 좌표 $p=[100,200]$, clean prediction $[104,192]$, 예측 잡음 $[0.25,-0.5]$, $\bar\alpha_t=0.25$, $\bar\alpha_s=0.64$로 둔다. 먼저 forward 관계로 만든 현재 상태는 $P_t=[102.216506\ldots,195.566987\ldots]$다. 이 $P_t$와 clean prediction을 예측 잡음 식에 넣으면 $[0.25,-0.5]$가 다시 나오고, update 뒤 좌표는 $P_s=[103.35,193.3]$이 된다. 이 값들은 스크립트로 검산한 교육용 예시이며 FFDP의 실제 schedule이나 출력이 아니다.

이 유도의 경계는 분명하다.

- FFDP 문헌은 DDIM 5 step을 사용한다고 밝힌다.
- FFDP가 실제로 $\eta=0$을 썼는지는 밝히지 않는다.
- 선택 timestep, $\alpha$ schedule, variance 설정도 공개 문헌에 없다.
- 따라서 위 식은 **[설명용 유도]**이며 FFDP 실제 sampler의 확정 구현이 아니다.

초기 표본을 뽑은 뒤 매 step에서 detector를 다시 실행하거나 가우시안에서 독립 표본을 다시 뽑는 것도 아니다. 현재 상태를 network가 정제해 다음 상태로 넘긴다. reliable keypoint는 그 과정에서 원래 관측 좌표로 유지된다.

---

## 8. Stage 1은 2D 복원 규칙을 학습한다

이제 수식을 module과 loss에 연결할 수 있다.

**[논문 원문] Stage 1의 전체 흐름. [보충자료] Encoder와 2D decoder의 구성.**

```text
GT 2D keypoint + detector 좌표 + noise level
    → noisy 2D sequence P_t 구성
    → 고정 DWT
    → spatial Transformer / temporal Transformer 기반 prior encoder
    → 2D decoder: LayerNorm + Linear
    → refined coefficient × coefficient map
    → 고정 iDWT
    → clean 2D prediction P_0^pred
    → L1 keypoint loss
```

**[논문 원문] 식 (6)**

$$
\mathcal L_{keyp}=|P-\hat P|.
$$

논문은 이를 $L_1$ keypoint loss라고 설명한다. 합인지 평균인지 같은 reduction 세부는 명시하지 않는다.

학습 표본과 예측 함수를 함께 쓰면 역할이 더 잘 보인다.

**[설명용 유도] Stage 1의 계산 관계를 압축한 표기**

$$
P_0^{\mathrm{pred}}
=\operatorname{iDWT}\!\left(
F_\theta(\operatorname{DWT}(P_t))
\right),
\qquad
\mathcal L_{2D}
=\lVert P_0^{\mathrm{pred}}-P_0^{GT}\rVert_1.
$$

$F_\theta$는 실제 논문의 한 함수 이름이 아니라 prior encoder, 2D decoder, coefficient scaling을 묶어 나타낸 설명용 기호다.

Stage 1에서 학습되는 것은 prior encoder와 2D decoder의 parameter다. DWT와 iDWT filter는 고정 연산이라 업데이트할 parameter가 없다. ViTPose detector까지 이 loss로 공동 학습한다는 근거도 없다.

**[보충자료]** prior encoder는 spatial information과 temporal information을 위한 서로 분리된 두 Transformer를 사용한다. **2D decoder는 Transformer가 아니라 LayerNorm과 Linear로 구성된다.** head 수, layer 수, embedding dimension은 공개 문헌에 명시되지 않았다.

---

## 9. Stage 2는 고정한 prior를 3D 복원에 사용한다

Stage 1이 끝나면 논문은 학습된 prior encoder를 고정한다. 이어 3D Transformer decoder가 3D motion의 wavelet subband와 shape parameter를 예측한다.

**[논문 원문] 식 (7)**

$$
Y_{h,v},\beta=D(y,z,I).
$$

- $y$: DWT로 분해한 2D 입력 subband
- $z$: 고정된 prior encoder가 만든 latent embedding
- $I$: RGB image에서 추출한 ViT feature
- $Y_{h,v}$: 3D motion의 예측 subband
- $\beta$: 사람의 shape parameter

예측한 3D subband는 iDWT를 거쳐 motion parameter가 된다.

**[논문 원문] 식 (8)**

$$
x=\operatorname{iDWT}(Y).
$$

논문의 사전 정의에서 $x$는 pose $\theta$와 translation $\tau$로 구성된다. SMPL을 통해 3D joint와 vertex를 얻고, 카메라로 2D에 재투영한다.

**[논문 원문] 식 (9)**

$$
\mathcal L
=\mathcal L_{smpl}
+\mathcal L_{joint}
+\mathcal L_{verts}
+\mathcal L_{keyp}.
$$

**[논문 원문] 손실 항의 정의**

$$
\mathcal L_{smpl}
=\lVert[\beta,\theta]-[\hat\beta,\hat\theta]\rVert_2^2,
$$

$$
\mathcal L_{joint}
=\lVert J_{3D}-\hat J_{3D}\rVert_2^2,
\qquad
\mathcal L_{vert}
=\lVert V_{3D}-\hat V_{3D}\rVert_2^2.
$$

$\mathcal L_{keyp}$는 재투영한 2D keypoint에 식 (6)의 $L_1$ loss를 적용한다. 논문 식에는 네 항의 단순 합으로 적혀 있지만, 실제 reduction과 loss weight는 공개 문헌에 명시되지 않았다. 이를 모두 구현 가중치 1이라고 단정하지 않는다.

**[보충자료]** 재투영에는 CLIFF perspective camera를 사용한다. principal point는 image center, focal length는 $\sqrt{w^2+h^2}$로 추정한다.

Stage 2에서 확실히 말할 수 있는 업데이트 범위는 다음과 같다.

| 구성요소 | Stage 1 | Stage 2 |
|---|---|---|
| DWT/iDWT filter | 고정 | 고정 |
| Prior encoder | 2D loss로 학습 | 논문에서 freeze 명시 |
| 2D decoder | 2D loss로 학습 | Stage 2의 사용·동결 범위 미명시 |
| 3D Transformer decoder | 해당 없음 | 3D·재투영 loss로 학습 |
| RGB ViT feature extractor | 해당 없음 | freeze/update 여부 미명시 |
| ViTPose detector | 입력 keypoint 제공 | 공동 학습 근거 없음 |

“Stage 2에서 prior encoder를 고정한다”는 사실로 “ViT도 고정한다”거나 “ViT는 반드시 3D decoder와 함께 fine-tuning한다”고 결론 내릴 수 없다.

---

## 10. 고정된 iDWT를 지나도 gradient는 흐른다

고정된 연산에는 학습할 parameter가 없다. 그렇다고 그 연산의 앞쪽까지 gradient가 갈 수 없다는 뜻은 아니다.

iDWT를 고정 선형 변환 $A$로 단순화하고, decoder 출력 coefficient를 $y_\theta$라고 하자.

**[설명용 유도] 고정 선형 변환을 지나는 연쇄법칙**

$$
x=Ay_\theta,
\qquad
\frac{\partial\mathcal L}{\partial y_\theta}
=A^\top\frac{\partial\mathcal L}{\partial x}.
$$

$A$ 자체를 바꾸지는 않지만, $A^\top$을 통해 decoder 출력에 대한 gradient를 계산할 수 있다. 그래서 좌표 영역이나 SMPL 결과에 둔 loss로 주파수 영역 coefficient를 예측한 decoder를 학습할 수 있다.

Stage 1에서는 $L_1$ loss의 gradient가 iDWT와 coefficient scaling을 지나 2D decoder와 prior encoder로 흐른다. Stage 2에서는 SMPL·joint·vertex·reprojection loss의 gradient가 3D decoder 쪽으로 흐르지만 prior encoder의 parameter는 freeze되어 업데이트되지 않는다.

여기서도 계산 가능성과 실제 optimizer 설정은 구분해야 한다. 문헌은 encoder freeze를 밝히지만, 공개 코드가 없으므로 `detach` 위치, optimizer parameter group, ViT의 update 여부를 확인할 수 없다.

---

## 계산을 끝내고 다시 보는 전체 경로

처음에는 DWT가 평균과 차이를 만든다는 사실만 보였다. 전체 경로를 따라가면 각 단계의 책임이 나뉜다.

1. DWT는 좌표 변화를 네 부대역으로 펼친다.
2. Prior network는 refined coefficient와 coefficient map을 예측한다.
3. $m\odot\hat y$는 각 위치의 coefficient를 학습된 배율로 조절한다.
4. iDWT는 조절된 부대역을 다시 좌표로 복원한다.
5. 검출 좌표 중심 가우시안은 불신뢰 관절의 초기 표본을 만든다.
6. Forward 식은 학습 때 GT와 detector 좌표 사이의 noisy sample을 만든다.
7. DDIM은 같은 36프레임 배열을 여러 noise level에서 반복해 정제한다.
8. Stage 1은 2D 복원 prior를 학습하고, Stage 2는 그 encoder를 고정해 3D decoder를 학습한다.

이제 $-8$ 같은 큰 high-frequency coefficient를 다시 보면 질문도 달라진다. “이 값이 잡음인가”를 DWT 하나에 묻는 대신, **어떤 주변 관측을 조건으로 network가 이 위치의 배율을 어떻게 조절했는가**를 물어야 한다.

아직 재현을 위해 남은 정보도 많다. wavelet family와 boundary mode, joint ordering, coefficient map의 범위, Transformer 세부 구조, noise schedule, DDIM의 $\eta$와 selected timestep은 공개 문헌에서 확인되지 않았다. 관절 순서를 바꾸거나 frame을 shift했을 때 coefficient가 어떻게 달라지는지, 실제 급변과 검출 오류가 어떤 부대역에서 구분되는지도 아직 수행하지 않은 후속 실험이다.

이 미확인 항목을 구현 기본값으로 채우지 않는 것이 중요하다. 현재 확인할 수 있는 수식과 학습 경로는 분명하지만, 독립 재현에서 선택한 값은 논문의 확정 설정과 따로 기록해야 한다.

---

## 참고 자료

- Buzhen Huang et al., [*Occluded Human Body Capture with Frequency Domain Denoising Prior*](https://openaccess.thecvf.com/content/CVPR2026/html/Huang_Occluded_Human_Body_Capture_with_Frequency_Domain_Denoising_Prior_CVPR_2026_paper.html), CVPR 2026.
- [FFDP 공식 본문 PDF](https://openaccess.thecvf.com/content/CVPR2026/papers/Huang_Occluded_Human_Body_Capture_with_Frequency_Domain_Denoising_Prior_CVPR_2026_paper.pdf)
- [FFDP 공식 보충자료 PDF](https://openaccess.thecvf.com/content/CVPR2026/supplemental/Huang_Occluded_Human_Body_CVPR_2026_supplemental.pdf)
- Jiaming Song, Chenlin Meng, Stefano Ermon, [*Denoising Diffusion Implicit Models*](https://arxiv.org/abs/2010.02502). 이 글의 deterministic DDIM 식은 원 논문의 구조를 FFDP residual 좌표에 적용한 `[설명용 유도]`다.
