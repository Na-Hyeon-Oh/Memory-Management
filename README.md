# Virtual Memory Management
✅ OS_PA3

<br>

## Index
+ [Overview](#ov)
+ [Tools](#with)
+ [Prerequisites](#pre)
+ [Details](#details)

<br><br>

## Overview <a name = "ov"></a>
Implementation of Page Replacement Policies for Demand Paging System.
- MIN
- FIFO
- LRU
- LFU
- WS


<br>

## Tools <a name = "with"></a>

<a href="https://github.com/search?q=user%3ADenverCoder1+is%3Arepo+language%3Ac"><img alt="C" src="https://img.shields.io/badge/C-2370ED.svg?logo=c&logoColor=white"></a>

✔️ WSL Ubuntu

✔️ cmder


<br>

## Prerequisites <a name = "pre"></a>

|Policy|Descripton|
|------|---|
|MIN|main file|
|FIFO|bash script|
|LRU|execution file|
|LFU|test cases|
|WS|test cases|

<br>

## Details <a name = "details"></a>

### Description of Files

|File Name|Descripton|
|------|---|
|OS43_2021-1_2019311923_OhNaHyeon.c|main file|
|Makefile|bash script|
|memManagement|execution file|
|input files|test cases|

### Compile and Execute

```
make
./memManagement
```

### Algorithm

|Policy|Idea|
|------|---|
|MIN| ⚫ Page frame에 할당되어 있는 page reference들을 대상으로 future-used까지의 remain time을 remainT(lookup table)에 저장 <br> ⚫ Tie-breaking : smallest page frame #부터 replace <br>⚫ 만약 page fault가 발생하지 않는다면 (할당된 page reference들 중에 같은 reference value 찾는 경우) 다음 page reference 검사<br>⚫ 만약 page fault가 발생 (할당된 page reference들 중에 같은 reference value 찾지 못하였을 경우) <br>  - 할당된 page frame # < M : 가능한 다음 page frame #에 해당 page reference 할당 <br>  - 할당된 page frame # == M (가득 참) : remainT에서 그 값이 최대 (다음 사용까지 시간이 많이 남음)인 page reference로 replace <br> ▶️ 다음 page reference 검사|
|FIFO| ⚫ Page frame을 Queue로 구현 -> head, tail 값을 조정하며 page reference allocate / replace<br>  ⚫ 만약 page fault가 발생하지 않는다면 (할당된 page reference들 중에 같은 reference value 찾는 경우) 다음 page reference 검사<br> ⚫ 만약 page fault가 발생 (할당된 page reference들 중에 같은 reference value 찾지 못하였을 경우) <br>  - 할당된 page frame # < M : 가능한 다음 page frame #에 해당 page reference 할당(enqueue) <br>  - 할당된 page frame # == M (가득 참) : memory Queue에서 가장 먼저 할당된 page reference를 replace<br> ▶️ 다음 page reference 검사|
|LRU| ⚫ Page frame에 할당되어 있는 page reference들을 대상으로 timestamp table인 time(lookup table)에 저장<br> ⚫ 만약 page fault가 발생하지 않는다면 (할당된 page reference들 중에 같은 reference value 찾는 경우) 다음 page reference 검사<br> ⚫ 만약 page fault가 발생 (할당된 page reference들 중에 같은 reference value 찾지 못하였을 경우) <br>- 할당된 page frame # < M : 가능한 다음 page frame #에 해당 page reference 할당<br>- 할당된 page frame # == M (가득 참) : time에서 그 값이 최소 (past-used t가 가장 작음)인 page reference로 replace<br> ▶️ 다음 page reference 검사|
|LFU| ⚫ Page frame에 할당되어 있는 page reference들을 대상으로 현재까지 사용된 횟수를 cnt(lookup table)에 저장<br> ⚫ Tie-breaking rule (같은 최소 cnt를 가지는 경우) : LRU<br> ⚫ timestamp table인 time(lookup table)<br> ⚫ 만약 page fault가 발생하지 않는다면 (할당된 page reference들 중에 같은 reference value 찾는 경우) 다음 page reference 검사<br> ⚫ 만약 page fault가 발생 (할당된 page reference들 중에 같은 reference value 찾지 못하였을 경우)<br>- 할당된 page frame # < M : 가능한 다음 page frame #에 해당 page reference 할당<br>- 할당된 page frame # == M (가득 참) : cnt에서 그 값이 최소 (past-used 횟수가 가장 작음)인 page reference로 replace, 만약 같은 값이 여러 개라면 time에서 그 값이 최소인 page reference로 replace<br> ▶️ 다음 page reference 검사|
|WS| ⚫ Page frame(memory)를 Queue처럼 구현; FIFO 방식과 비슷<br>- Queue에 매 t마다 page reference를 enqueue<br>- Queue의 head와 tail을 W를 고려하여 그 간격을 일정하게 이동시킴<br> ⚫ 전 t에 memory에 할당되었던 page reference를 traceWS에 저장 : traceWS { int cnt //size of last + int* last //page reference array allocated at (t-1)} <br> ⚫ page fault가 발생 (현 t의 page reference와 last를 cnt(->allocated)만큼 비교하여 같은 것이 있는 경우) / page fault가 발생 (할당된 page reference들 중에 같은 reference value 찾지 못하였을 경우)하는 경우 모두에서 memory Queue의 head, tail 값을 알맞게 조정|
