---
layout: post
title: "SQLite 3강 - 조류 GPS 데이터 분석"
description: "6만 건 실데이터로 SQL 실습 — CSV 마이그레이션부터 JOIN 집계까지"
date: 2026-02-21
category: "SQL"
tags: sql, sqlite, join, group-by, migration
comments: true
---

## 들어가며

이번 강의는 1, 2강에서 배운 SQL을 **실제 데이터**에 적용하는 실습이었다. 조류 GPS 추적 데이터 약 6만 건을 다루는데, 장난감 데이터가 아니라 진짜 데이터를 다루니까 SQL 함수들의 실용성이 체감됐다. 특히 `substr`로 날짜를 파싱하고 `GROUP BY`로 월별 집계하는 패턴이 실무에서 정말 많이 쓰인다고 한다.

---

## CSV → 데이터베이스 마이그레이션

Python의 `pandas`와 `sqlite3`로 CSV를 DB에 넣을 수 있다. DB Browser에서 GUI로 할 수도 있지만, 코드로 하면 자동화가 가능하다.

```python
import pandas as pd
import sqlite3

# DB 연결 (없으면 자동 생성)
conn = sqlite3.connect('./SQL/bird.db')

# bird_tracking_devices.csv (7건 - 새 기기 정보)
devices = pd.read_csv('./SQL/bird_tracking_devices.csv')
devices.to_sql('bird_tracking_devices', conn, if_exists='replace', index=False)
print(f"bird_tracking_devices: {len(devices)}건 완료")

# bird_tracking.csv (61,920건 - GPS 추적 기록)
tracking = pd.read_csv('./SQL/bird_tracking.csv')
tracking.to_sql('bird_tracking', conn, if_exists='replace', index=False)
print(f"bird_tracking: {len(tracking)}건 완료")

conn.close()
print("마이그레이션 완료!")
```

테이블 구성:
- `bird_tracking_devices` — 추적 장치 정보 (종, 포획 위치, 장치 번호, 성별, 학명, 이름)
- `bird_tracking` — GPS 추적 데이터 (장치 번호, 위치, 시간 등 약 6만 건)

---

## 사용된 함수 및 키워드 정리

집계 함수:
- `count(*)` — 전체 행 수 / `count(distinct 컬럼)` — 중복 제거 후 행 수
- `min(컬럼)`, `max(컬럼)` — 최소/최대

문자열 함수:
- `substr(문자열, 시작, 길이)` — 부분 추출. `substr(date_time, 1, 7)` → `2013-08`

> SQLite는 날짜 형식을 직접 제공하지 않아서, `substr`로 텍스트를 잘라 년월을 추출한다.

주요 키워드:
- `rowid` — SQLite 내부 자동 행 번호
- `distinct` — 중복 제거
- `group by` — 그룹화
- `order by` — 정렬

---

## 실습: 데이터 탐색

### 첫 번째 레코드 조회 (rowid 활용)

```sql
select species_code, catch_location, device_info_serial,
       sex, scientific_name, ring_code, bird_name
from bird_tracking_devices
where rowid = 1;
```

결과: 종 코드 `hg`(재갈매기), 벨기에 오스텐트에서 포획, 801번 장치, 이름 Jurgen.

### 특정 장치 번호로 조회

```sql
select sex 성별,
       scientific_name 학명,
       bird_name 이름,
       tracking_started_at 추적시작일시,
       tracking_ended_at 추적종료일시
from bird_tracking_devices
where device_info_serial = 851;
```

851번 장치 = 검은등갈매기 수컷 Eric. 추적종료일시가 NULL → 추적 진행 중.

### 총 건수와 추적장치 개수

```sql
select count(*) 총건수,
       count(distinct device_info_serial) 추적장치개수
from bird_tracking;
```

6만 건 넘는 데이터가 3개 추적장치에서 수집됐다.

### 특정 개체의 추적 기간

```sql
select count(*) 건수
from bird_tracking
where device_info_serial = 851;
-- 결과: 19795
```

```sql
select min(date_time) 시작일시,
       max(date_time) 종료일시
from bird_tracking
where device_info_serial = 851;
-- 결과: 2013-08-15 ~ 2014-04-30
```

`min()`/`max()`로 기간의 시작과 종료를 파악하는 패턴은 실무에서도 자주 쓰인다.

---

## 실습: 집계와 JOIN

### 추적장치별, 월별 집계

```sql
select device_info_serial 추적장치일련번호,
       substr(date_time, 1, 7) 년월,
       count(*) 건수
from bird_tracking
group by 추적장치일련번호, 년월
order by 추적장치일련번호, 년월;
```

`substr` + `group by` 조합이 SQLite에서 날짜 기반 집계의 핵심 패턴이다.

### JOIN으로 장치번호 대신 새 이름 표시

장치번호만 보면 누가 누군지 모르니까, JOIN으로 이름을 가져온다:

```sql
select d.bird_name 이름,
       substr(t.date_time, 1, 7) 년월,
       count(*) 건수
from bird_tracking t
join bird_tracking_devices d
  on t.device_info_serial = d.device_info_serial
group by 이름, 년월
order by 이름, 년월;
```

---

## 헷갈리기 쉬운 포인트

### GROUP BY에 별명을 쓸 수 있는 건 SQLite 특성

`group by 이름, 년월`처럼 SELECT에서 정의한 별명을 GROUP BY에 바로 쓸 수 있는 건 **SQLite의 확장 기능**이다. MySQL에서도 되지만, 표준 SQL에서는 안 되는 경우가 있다. 다른 DBMS를 쓸 때는 주의.

### count(*) vs count(distinct ...) — 언제 뭘 쓰나

- `count(*)` — 단순히 행이 몇 개인지
- `count(distinct 컬럼)` — **고유한 값**이 몇 종류인지

"추적장치가 몇 개야?"는 `count(distinct device_info_serial)`, "데이터가 총 몇 건이야?"는 `count(*)`.

### rowid는 직접 만든 게 아니다

`rowid`는 SQLite가 모든 테이블에 자동으로 부여하는 내부 행 번호다. PRIMARY KEY를 INTEGER로 설정하면 rowid와 동일하게 동작한다. 데이터 탐색 시 유용하지만, 행이 삭제되면 번호가 재사용될 수 있으므로 ID 용도로는 직접 PRIMARY KEY를 만드는 게 안전하다.

---

## 정리하며

1. **CSV → DB 마이그레이션**은 pandas + sqlite3로 코드 몇 줄이면 된다
2. `substr` + `group by`는 SQLite 날짜 집계의 핵심 패턴
3. `min()`/`max()`로 기간 파악, `count(distinct)`로 고유 값 개수 세기
4. **JOIN**으로 여러 테이블을 연결하는 게 관계형 DB의 핵심이다
5. 실제 데이터(6만 건)를 다루니까 SQL의 실용성이 확 체감된다
