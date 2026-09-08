---
layout: stack
title: Paper Review
category: Paper Review
description: 논문의 주장과 구현 사이에서 이해한 것, 아직 헷갈리는 것을 정리합니다.
permalink: /stack/paper-review/
---
<section class="learning-guide" id="vision-path">
  <h2>배경 개념에서 CIGPose 구현까지</h2>
  <p>각 논문을 따로 읽어도 좋고, 다음 순서로 배경을 짚어 가며 읽어도 좋습니다.</p>
  <ol class="path-steps">
    <li><a href="{{ '/2026/05/resnet.html' | relative_url }}">ResNet</a><span>깊은 신경망과 residual 연결</span></li>
    <li><a href="{{ '/2026/05/gcn.html' | relative_url }}">GCN</a><span>그래프에서 특징을 전달하는 방식</span></li>
    <li><a href="{{ '/2026/09/cigpose.html' | relative_url }}">CIGPose 논문 리뷰</a><span>가려진 관절을 추정하는 아이디어와 한계</span></li>
    <li><a href="{{ '/2026/09/cigpose-implementation.html' | relative_url }}">CIGPose 구현 해설</a><span>수식, 텐서, gradient를 코드로 연결하기</span></li>
  </ol>
</section>
<section class="learning-guide">
  <h2>테이블 데이터와 베이지안 추론이 궁금하다면</h2>
  <p>동전 던지기 예시에서 posterior predictive distribution과 Transformer 추론까지 따라갑니다.</p>
  <a href="{{ '/2026/06/tabpfn.html' | relative_url }}">TabPFN 리뷰 읽기 →</a>
</section>
