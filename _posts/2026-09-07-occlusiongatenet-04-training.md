---
layout: post
title: "OcclusionGateNet 개발기 4 — loss는 내려가는데 복원은 왜 안 될까?"
description: "정답 좌표 재점검, VQ codebook collapse, warm-start, alpha 보조 손실과 CPU 전처리 병목. 저장된 학습 일지를 현재 코드와 대조해 정리한다."
date: 2026-09-07
category: "Project"
subcategory: "Computer Vision / DMS"
tags: [deep-learning, training, debugging, vq-vae, orformer]
comments: true
series: occlusiongatenet
series_order: 4
series_title: "OcclusionGateNet 개발기"
permalink: /2026/09/occlusiongatenet-04-training.html
---

## 들어가며

지난 편에서는 ORFormer의 구조를 설명했다. regular와 messenger가 code를 예측하고, alpha로 두 표현을 섞어서 얼굴 구조를 만드는 모델이다.

그런데 학습을 돌리면 이 역할 분담이 저절로 생길까? 실제 기록은 그렇지 않았다. **codebook이 거의 한 번호만 사용하기도 했고, 가린 샘플과 정상 샘플의 alpha가 똑같아지기도 했다.** loss 하나만 보고 있었다면 무엇이 잘못되었는지 알아채기 어려웠을 것이다.

이번 편은 저장소의 `TRAINING_LOG.md`에 남아 있는 2026년 5월 학습을 정리한 글이다. 아래 숫자는 당시의 기록이며, 글을 쓰면서 다시 학습한 결과는 아니다. 학습 일지의 해석을 그대로 옮기기보다 현재 공개 코드로 확인되는 범위도 함께 적었다.

## Part 1. 모델보다 먼저, 정답 좌표를 의심했다

초기에는 기존 occface predictor의 좌표를 목표로 사용하려 했다. 그런데 얼굴에 좌표를 겹쳐 보는 검증 과정에서 맞지 않는 사례가 나왔고, MediaPipe를 다시 실행한 좌표와 비교했다.

기록된 두 예측기의 평균 L1 차이는 **112 좌표계에서 16.7px**, 256 크기로 환산하면 약 38px였다. 얼굴 crop 안에서 무시하기 어려운 차이다.

다만 이 숫자 자체가 ‘occface가 실제 정답에서 16.7px 틀렸다’는 뜻은 아니다. **두 예측기가 그만큼 서로 달랐다**는 결과다. 시각화까지 검토한 뒤, 프로젝트에서는 MediaPipe 출력을 pseudo-target으로 사용하는 방향으로 바꾸었다.

MediaPipe cache 생성 기록에는 약 **153만 프레임**, 검출률 **86.1%**, 소요 시간 **18.6분**이 남아 있다. 이 시간은 당시 환경과 병렬 처리 조건의 결과다. 검출률 역시 ‘나머지 프레임의 정답이 복구되었다’는 뜻이 아니라, pseudo-label을 얻지 못하는 샘플이 존재한다는 의미로 읽어야 한다.

> 학습 코드가 정상적으로 돌아가더라도 목표 좌표가 어긋나 있으면, 모델은 그 어긋난 목표에 가까워지는 방향으로 학습한다.

## Part 2. codebook 2,048개를 만들어 놓고 하나만 사용한다면

### Perplexity가 1에 가까워졌다

처음에는 codebook을 scratch로 학습하려 했다. 로그에는 약 **50 step 만에 perplexity가 1.0으로 무너졌다**고 기록되어 있다. 초기화의 크기를 키우고 beta를 바꾼 시도에서도 같은 현상이 반복되었다.

여기서 perplexity는 언어 모델의 문장 평가와 같은 맥락으로 읽기보다, **code 사용 분포의 다양성을 요약하는 값**으로 이해하면 된다. code $k$가 선택된 비율을 $p_k$라 두면 다음처럼 계산한다.

**[코드 기반 재구성 — quantizer의 code 사용 perplexity]**

$$
\operatorname{perplexity}
=\exp\!\left(-\sum_k p_k\log(p_k+\varepsilon)\right)
$$

하나의 code만 쓰면 1에 가까워진다. 여러 code를 고르게 쓰면 커진다. 단, perplexity가 10이라고 **정확히 10개 code만 사용했다**는 뜻은 아니다. 빈도가 치우친 분포의 유효 다양성을 나타내는 값이다.

2,048개 중 몇 개를 반드시 써야 한다는 정답도 없다. 하지만 학습 초반부터 거의 하나로 고정된다면, 입력마다 다른 구조를 표현하려던 목적과 어긋났는지 확인해야 한다.

### Reconstruction loss만으로는 부족했다

당시 진단에는 sparse한 edge heatmap 때문에 decoder가 단순한 출력으로도 loss를 줄일 가능성이 적혀 있다. 배경이 대부분인 목표에서는 전체 평균 오차가 작아져도 정작 얼굴 선을 잘 표현하지 못할 수 있다.

이것은 가능한 원인에 대한 당시의 해석이다. 초기화와 loss 가중치만 바꾸어도 해결되지 않았다는 관측은 있지만, collapse의 모든 원인을 분리한 실험은 아니다. 그래서 ‘VQ-VAE는 원래 학습이 안 된다’보다 **우리 데이터와 설정에서 scratch 학습이 안정화되지 않았다**고 정리하는 편이 맞다.

### Warm-start 이후의 변화

다음 시도에서는 기존 COFW 학습 체크포인트의 encoder·codebook·decoder를 가져왔다. DMD의 edge 수와 맞지 않는 마지막 decoder 층은 새로 초기화하고, codebook 쪽 learning rate를 작게 두었다.

| 시점 | Reconstruction | Commitment | Perplexity |
|---|---:|---:|---:|
| step 0 | 0.355 | 146.6 | 6.1 |
| step 100 | 0.0019 | 2.00 | 5.9 |
| step 500 | 0.0011 | 0.060 | 9.7 |
| step 1,000 | 약 0.0010 | 약 0.009 | 약 11~12 |

validation reconstruction은 **0.00108**, perplexity는 **10.36**으로 기록되어 있다. 처음부터 codebook을 새로 만드는 대신, 이미 형성된 표현을 NIR 입력에 맞추는 방향이 이 설정에서는 안정적이었다.

이 값이 최종 landmark 오차나 gaze F1은 아니다. 복원 파이프라인 안에서도 **heatmap reconstruction → landmark 좌표 → 운전자 분류**는 서로 다른 평가 단계다.

## Part 3. alpha가 양쪽에서 똑같아졌다

### Messenger를 사용할 이유가 충분했을까?

Phase 2의 초기 설정은 alpha 보조 손실의 가중치가 0.1이고, warmup은 10 epoch였다. 기록은 다음과 같다.

| 조건 | 정상 샘플의 평균 alpha | 가림 샘플의 평균 alpha | Train perplexity |
|---|---:|---:|---:|
| 초기 설정, epoch 0 | 0.076 | 0.075 | 5.3 |
| 초기 설정, epoch 1 | 0.039 | 0.039 | 3.2 |
| aux 0.5, warmup 0 | 0.0106 | 0.1992 | 28.7 |
| 위 결과로 warm-start, batch 64 | 0.006 | 0.204 | 29.2 |

초기에는 alpha가 작아지면서 두 종류의 샘플에서 거의 같은 값이 나왔다. 앞 편의 혼합식을 떠올리면, alpha가 작아질수록 regular 쪽 표현이 지배한다. 가림을 처리하려고 넣은 messenger 경로가 충분히 사용되지 않는 방향이었다.

그래서 보조 가중치를 **0.5**, warmup을 **0**으로 바꾸었다. 가림을 알려주는 supervision을 학습 시작부터 적용한 것이다. 이후 로그에서는 정상·가림 샘플 평균 사이에 차이가 생겼다.

### 어떤 loss를 더했나?

현재 Phase 2 스크립트의 loss는 다음처럼 정리할 수 있다.

**[코드 기반 재구성]**

$$
\mathcal L
=\mathcal L_{\mathrm{heatmap}}
+\mathcal L_{\mathrm{VQ}}
+\lambda_{\mathrm{aux}}\,\operatorname{BCE}(\alpha,y_{\mathrm{occ}})
$$

첫 항은 reference heatmap reconstruction, 두 번째는 quantization 관련 손실, 세 번째는 alpha에 가림 정보를 주는 보조 손실이다. VQ 항의 세부 가중치 배치는 원 VQ-VAE의 표기와 프로젝트 코드가 같다고 가정하지 않고 실제 `quantizer.py`를 기준으로 읽어야 한다.

중요한 제한도 있다. 위 표의 alpha는 **샘플 전체 token의 평균을 낸 뒤 normal/occluded 그룹으로 집계한 값**이다. 평균이 달라졌다고 개별 가림 위치를 정확하게 찾아냈다는 평가까지 끝난 것은 아니다.

### 글을 정리하며 확인한 남은 구현 문제

현재 공개된 `manifest_to_patch_16` 함수에는 **placeholder**가 남아 있다. 478개 landmark의 가시성 표시를 얼굴 공간의 16×16 grid로 정확히 투영하는 대신, 앞의 256개 값을 잘라 사용한다. 점이 적으면 뒤를 채우는 방식이다.

landmark 번호는 이미지의 raster 순서가 아니다. 따라서 이것을 ‘각 patch의 실제 가림 정답을 supervision했다’고 설명하면 부정확하다. 학습 일지에 남은 alpha 변화는 당시 관측으로 소개할 수 있지만, **현재 함수가 정확한 공간 정답을 구현한다는 근거로 사용할 수는 없다.**

다시 학습한다면 우선 landmark 위치와 crop 좌표를 이용해 실제 grid별 target을 만들고, 그 target을 이미지 위에 겹쳐 확인해야 한다. 이 시리즈를 쓰는 동안 연구 코드를 수정하거나 해당 실험을 재실행하지는 않았다. 남은 과제로 명확히 구분해둔다.

## Part 4. GPU보다 CPU를 먼저 고쳐야 했던 순간

당시 기록에는 50 step에 38분이 걸린 구간도 있다. 확인해보니 샘플마다 point heatmap을 만드는 `generate_pointmap`의 반복문이 약 **270ms/sample**을 사용하고 있었다.

이를 NumPy vectorization으로 바꾼 뒤 같은 처리 시간이 약 **16ms/sample**로 줄었다. 비율로는 약 17배다. DataLoader worker 수도 조절했다.

여기서 ‘학습이 17배 빨라졌다’고 쓰면 안 된다. **17배는 해당 전처리 함수의 측정값**이다. 실제 epoch 시간에는 GPU 계산, 다른 전처리, 데이터 전달, 공유 장비의 부하가 함께 들어간다. batch와 worker를 바꾼 후 epoch 시간이 달라진 기록도 있지만, 그것을 단일 함수 최적화의 효과로 모두 돌릴 수는 없다.

이 경험은 학습 속도를 볼 때 loss나 GPU 메모리만 확인해서는 부족하다는 것을 보여준다. GPU가 다음 batch를 기다리는 동안 CPU에서 무엇을 만들고 있는지도 학습 경로의 일부다.

## Part 5. 합성 appearance에 맞춘 HGNet fine-tuning

초기 가림 학습과 다른 checker·stripe·noise 패턴에서 좌표 오차가 커지는 문제를 다루기 위해, 8종 appearance로 HGNet을 추가 조정하는 스크립트도 있다.

현재 코드를 보면 이 단계에서는 **ORFormer를 고정하고 HGNet만 학습**한다. clean crop에 가림을 합성하되 목표는 clean MediaPipe 좌표이며, loss에는 좌표의 NME뿐 아니라 point·edge heatmap 보조 항도 포함된다. 따라서 README의 간단한 흐름도를 그대로 ‘NME 하나만으로 두 모델을 함께 fine-tuning’했다고 읽지 않았다.

또한 fine-tuning 문서는 해당 HGNet이 통합 시스템에 사용된다고 설명하지만, 모델 배치 문서에는 이전 버전 이름이 남아 있다. 체크포인트 본체 없이 문서 이름만으로 최종 배포 모델의 계보를 완전히 확정하기는 어렵다. 학습 기록, 설정 파일, 실제 로드한 체크포인트의 식별 정보를 함께 남겨야 하는 이유다.

여기까지는 **가림 입력에서 구조를 추정하는 모델을 학습한 과정**이었다. 다음 질문은 조금 다르다. 복원한 좌표가 있다고 해서, 분류기가 그 얼굴을 무조건 믿어도 될까? 다음 편에서는 좌표를 교체하는 gate와 feature를 섞는 gate를 나누어 살펴본다.

### 참고한 자료

- [학습 일지 — 수치와 당시 진단](https://github.com/yg2127/OcclusionGateNet/blob/17d4211887ea4bd4848a382b24569d76c96f9961/landmark/docs/TRAINING_LOG.md)
- [Phase 2 학습 스크립트 — 보조 loss와 임시 patch 매핑](https://github.com/yg2127/OcclusionGateNet/blob/17d4211887ea4bd4848a382b24569d76c96f9961/landmark/scripts/train_phase2_orformer.py)
- [HGNet fixedmask fine-tuning](https://github.com/yg2127/OcclusionGateNet/blob/17d4211887ea4bd4848a382b24569d76c96f9961/pipeline/finetune_hgnet_fixedmask.py)
- [배포 모델 목록](https://github.com/yg2127/OcclusionGateNet/blob/17d4211887ea4bd4848a382b24569d76c96f9961/models/MODELS.md)
