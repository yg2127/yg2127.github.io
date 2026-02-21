---
layout: post
title: "SQL 입문 3강 - 조류 GPS 데이터 분석"
description: "CSV 마이그레이션, rowid, count distinct, substr, GROUP BY, JOIN 실습"
date: 2026-02-21
category: "SQL"
tags: sql, sqlite, join, group-by, migration
comments: true
---

## 잘한 점

### 상황 1.

- **상황**: 조류 GPS 데이터를 데이터베이스에 넣어야 했다
- **액션**: DB Browser를 사용하지 않고 VS Code에서 직접 Python 코드를 통해 CSV → DB 마이그레이션을 수행했다
- **칭찬**: 도구에 의존하지 않고 코드로 직접 처리한 점이 좋았다

---

## 개선점

### 상황 1.

- **문제**: 실습 예제 외에 내가 원하는 대로 데이터를 불러오지 못했다
- **원인**: 다양한 SQL 함수 문법에 아직 익숙하지 않다
- **액션플랜**: 더 다양한 예제를 실습하고 정확한 함수 문법을 공부하기

---

## 배운 점

### 상황 1. CSV → 데이터베이스 마이그레이션

- **배움**: Python의 `pandas`와 `sqlite3`를 사용하여 CSV 파일을 데이터베이스로 마이그레이션할 수 있다.

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

사용된 테이블:
- `bird_tracking_devices` — 추적 장치 정보 (종, 포획 위치, 장치 번호, 성별, 학명, 가락지 번호, 이름)
- `bird_tracking` — GPS 추적 데이터 (장치 번호, 위치, 시간 등 약 6만 건)

- **의미**: GUI 도구 없이 코드만으로 데이터를 DB에 넣는 방법을 익혀두면 자동화에 활용할 수 있다

### 상황 2. 사용된 함수 및 키워드

- **배움**: 이번 실습에서 사용한 주요 함수와 키워드를 정리한다.

집계 함수:
- `count(*)` — 전체 행 수 계산
- `count(distinct 컬럼)` — 중복 제거 후 행 수 계산
- `min(컬럼)` — 최솟값 반환
- `max(컬럼)` — 최댓값 반환

문자열 함수:
- `substr(문자열, 시작, 길이)` — 문자열 부분 추출. `substr(date_time, 1, 7)` → `2013-08`

> SQLite는 날짜 형식을 직접 제공하지 않아서, `substr`로 텍스트를 잘라 년월을 추출한다.

SQL 키워드:
- `select` — 조회할 컬럼 지정
- `from` — 대상 테이블 지정
- `where` — 조건절
- `group by` — 그룹화 (집계 함수와 함께)
- `order by` — 정렬
- `distinct` — 중복 제거
- `rowid` — SQLite 내부 자동 행 번호
- 컬럼 별명 — `select sex 성별` 또는 `select sex as 성별`

- **의미**: 함수와 키워드를 한눈에 정리해두면 쿼리 작성 시 빠르게 참고할 수 있다

### 상황 3. 첫 번째 레코드 조회 (rowid 활용)

- **배움**: SQLite의 `rowid`를 활용하여 특정 행에 접근할 수 있다.

```sql
select species_code, catch_location, device_info_serial,
       sex, scientific_name, ring_code, bird_name
from bird_tracking_devices
where rowid = 1;
```

- 종 코드 `hg` = 재갈매기(Herring Gull)
- 벨기에 오스텐트(Oostende)에서 포획
- 801번 추적장치 부착, 가락지 번호 H903185, 이름 Jurgen

- **의미**: `rowid`는 별도로 정의하지 않아도 SQLite가 자동으로 부여하는 행 번호이므로, 데이터 탐색 시 유용하다

### 상황 4. 특정 장치 번호로 조회 + 컬럼 별명

- **배움**: `where` 조건으로 특정 데이터를 필터링하고, 컬럼 별명으로 결과 헤더를 변경할 수 있다.

```sql
select sex 성별,
       scientific_name 학명,
       bird_name 이름,
       tracking_started_at 추적시작일시,
       tracking_ended_at 추적종료일시
from bird_tracking_devices
where device_info_serial = 851;
```

- 851번 장치 = 검은등갈매기(Larus fuscus) 수컷 Eric
- 추적종료일시가 NULL → 추적 진행 중

- **의미**: 컬럼 별명을 한글로 지정하면 결과를 직관적으로 파악할 수 있다

### 상황 5. 총 건수와 추적장치 개수 (count, count distinct)

- **배움**: `count(*)`와 `count(distinct ...)`의 차이를 실습했다.

```sql
select count(*) 총건수,
       count(distinct device_info_serial) 추적장치개수
from bird_tracking;
```

- 6만 건이 넘는 데이터가 3개 추적장치에서 수집되었다

- **의미**: `count(distinct ...)`로 중복을 제거한 고유 값 개수를 셀 수 있다

### 상황 6. 특정 개체의 데이터 건수와 추적 기간

- **배움**: `count`와 `min`/`max`를 조합하여 특정 개체의 데이터 건수와 추적 기간을 조회할 수 있다.

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

- **의미**: `min()`/`max()`로 기간의 시작과 종료를 파악하는 패턴은 실무에서 자주 쓰인다

### 상황 7. 추적장치별, 월별 집계 (substr + group by + order by)

- **배움**: `substr`로 년월을 추출하고 `group by`로 집계하는 방법을 실습했다.

```sql
select device_info_serial 추적장치일련번호,
       substr(date_time, 1, 7) 년월,
       count(*) 건수
from bird_tracking
group by 추적장치일련번호, 년월
order by 추적장치일련번호, 년월;
```

- `substr(date_time, 1, 7)` → `"2013-08-15 ..."` 에서 `"2013-08"` 추출
- `group by`로 장치별+월별 그룹화
- `order by`로 장치번호, 년월 순 정렬

- **의미**: `substr` + `group by` 조합은 SQLite에서 날짜 기반 집계를 할 때 핵심 패턴이다

### 상황 8. JOIN을 활용한 연습문제

- **배움**: `device_info_serial` 대신 `bird_name`이 나타나도록 두 테이블을 JOIN하여 집계하는 연습문제를 풀었다.

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

- 두 테이블을 `device_info_serial`로 JOIN하여 장치 번호 대신 새 이름으로 표시한다

- **의미**: JOIN을 통해 여러 테이블의 정보를 결합하는 것이 관계형 데이터베이스의 핵심이다

### 상황 9. 핵심 정리

- **배움**: 이번 강의의 핵심 내용을 정리한다.

1. **`rowid`** — SQLite가 자동 부여하는 행 번호. `where rowid = 1`로 첫 행 접근
2. **컬럼 별명** — `select 컬럼 별명` 형태로 결과 헤더를 한글로 변경 가능
3. **`count(distinct ...)`** — 중복을 제거한 고유 값 개수 세기
4. **`substr()`** — SQLite에 날짜 함수가 없을 때 텍스트 조작으로 년월 추출
5. **`group by` + `order by`** — 집계 결과를 그룹화하고 정렬
6. **`min()` / `max()`** — 기간(시작~종료) 파악에 활용

- **의미**: 실제 데이터(6만 건)를 다루면서 SQL 함수들의 실용성을 체감할 수 있었다
