run_algo.py: to run all algorithms over all tracesfiles provided.
merge_sort.c: a simple merge sort algorithm to sort a list of integers recursively.

One paragraph comparing the various algorithms in terms of the results you see in the tables.

rand: totally random when choosing the evcition, it does perform better when memory size increases,
but for most of the time, it's not as good as others.
fifo: the first-in page usually has higher probability then others to finish its job(aka, safe to evict it),
but not all of them have this property. However, it's as simple as random algorithm, and slightly better
than rand in most of cases.
clock: to me, clock has similar concept as lru, but it evicts 'relatively' least-recent-used, because it uses
bit to indicate not exact visit time. So clock will always perform a little bit worst than lru, except that 
for tr-blocked.ref when memory size is 200, clock performs a little bit better than lru.
lru: 'global' least-recent-used as it keeps track of exact visit time, the best we can do in practical,
most close one to opt when comparing results.
opt: best in all cases(theoretically), however, we are not able to achieve this in practical. But it's good to use opt algorithm as an indicator.
Overall, all algorithms perform better when memory size increases(then number of eviction needed will decrease).
I would rank them as following:
opt > lru >= clock > fifo > rand

A second paragraph explaining the data you obtained for LRU as the size of memory increases.

for lru, the hit rate will increase as the memory size increases, but the increasing rate will decay(slower
and slower) after reaching a point. It seems like lru and opt both will converge at certain point, but they have 
different point. That being said, in general, lru can never reach the theoretically optimal(as good as opt), no matter how big is the memory size.



trace file:traceprogs/tr-simpleloop.ref
algorithm:rand
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		71.8726 		7607 		2977 				2927 				226 		2701 
		100		74.0174 		7834 		2750 				2650 				52 			2598 
		150		74.3386 		7868 		2716 				2566 				21 			2545 
		200		74.5181 		7887 		2697 				2497 				14 			2483

algorithm:fifo
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		71.9766 		7618 		2966 				2916 				205 		2711 
		100		74.0079 		7833 		2751 				2651 				44 			2607 
		150		74.3859 		7873 		2711 				2561 				16 			2545 
		200		74.4615 		7881 		2703 				2503 				12 			2491 

algorithm:lru
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		73.7528 		7806 		2778 				2728 				91 			2637 
		100		74.6882 		7905 		2679 				2579 				0 			2579 
		150		74.7071 		7907 		2677 				2527 				0 			2527 
		200		74.7071 		7907 		2677 				2477 				0 			2477 

algorithm:clock
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		73.6961	 		7800 		2784 				2734 				92 			2642 
		100 	74.6599 		7902 		2682 				2582 				3 			2579 
		150		74.7071 		7907 		2677 				2527 				0 			2527 
		200 	74.7071 		7907 		2677 				2477 				0 			2477 

algorithm:opt
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50 		74.8016 		7917 		2667 				2617 				20 		 	2597 
		100 	75.1134  		7950  		2634 	 	 		2534  	 			0 	 	 	2534 
 		150 	75.1134  		7950  	 	2634   	 			2484  	 			0  	 		2484 
 		200 	75.1134 	 	7950 	 	2634  	 			2434  	 			0  		 	2434 
======================================================================================================

trace file:traceprogs/tr-matmul.ref
algorithm:rand
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		66.4846 	1973381 	994795 				994745 				955639 			39106 
		100		89.1120 	2645001 	323175 				323075 				315688 			7387 
		150		96.7515 	2871754 	96422 				96272 				93946 			2326 
		200		98.1026 	2911859 	56317 				56117 				54467 			1650 

algorithm:fifo
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		62.0215 	1840906 	1127270 			1127220 			1083235 		43985 
		100		63.4944 	1884627		1083549 			1083449 			1061225 		22224 
		150		98.8408 	2933769		34407 				34257 				32946 			1311 
		200		98.8584 	2934291		33885 				33685 				32435		 	1250 

algorithm:lru
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		64.9204 	1926951 	1041225 			1041175 			1040073 		1102 
		100 	66.0920 	1961728 	1006448 			1006348 			1005271 		1077 
		150 	98.8922 	2935296 	32880 				32730 				31655 			1075 
		200	 	98.8926 	2935307 	32869 				32669 				31594 			1075 

algorithm:clock
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		64.9200 	1926939 	1041237 			1041187 			1040081 		1106 
		100 	66.2491 	1966389 	1001787 			1001687 			1000610 		1077 
		150	 	98.8321 	2933510 	34666 				34516 				33439		 	1077 
		200 	98.8922 	2935294 	32882 				32682 				31607 			1075 

algorithm:opt
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
 		50 	 	80.2083 	2380724 	587452  	 		587402  			586320 	 		1082 
 		100	 	96.8738  	2875384  	92792  		  		92692  		 		91613  			1079 
 		150	 	99.1035 	2941567  	26609 	 		 	26459  	 			25381 	 		1078 
 		200	  	99.3512 	2948917 	19259 		 	  	19059  		 		17981  			1078 
======================================================================================================

trace file:traceprogs/tr-blocked.ref
algorithm:rand
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50	 	99.6672 	2516645 	8403 				8353 				5856 			2497 
   		100		99.7908 	2519766 	5282 				5182 				3411 			1771 
		150	 	99.8242 	2520608 	4440 				4290 				2780 			1510 
		200	 	99.8473 	2521193 	3855 				3655 				2334 			1321 

algorithm:fifo
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		99.7445 	2518596 	6452 				6402 				4151 			2251 
		100 	99.8282 	2520709 	4339 				4239 				2762 			1477 
 		150		99.8320 	2520805 	4243 				4093 				2672 			1421 
 		200		99.8741 	2521868 	3180 				2980 				1885 			1095 

algorithm:lru
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50	 	99.7946 	2519861 	5187 				5137 				2791 			2346 
		100		99.8490 	2521236 	3812 				3712 				2632 			1080 
		150		99.8493 	2521244 	3804 				3654 				2596 			1058 
		200 	99.8536 	2521352 	3696 				3496 				2438 			1058 

algorithm:clock
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		99.7715 	2519279 	5769 				5719 				3303 			2416 
		100 	99.8346 	2520871 	4177 				4077 				2641 			1436 
 		150		99.8492 	2521239 	3809 				3659 				2600 			1059 
 		200		99.8726 	2521830 	3218 				3018 				1955 			1063 

algorithm:opt
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		 50 	99.8526 	2521326  	3722 	 		 	3672 	 			2588 	 		1084 
		100 	99.8804 	2522029  	3019  				2919 				1846 			1073 
		150	 	99.8998 	2522518		2530 				2380 				1305 			1075 
		200 	99.9097 	2522768		2280 				2080 				1014 			1066 
======================================================================================================

trace file:traceprogs/tr-merge_sort.ref
algorithm:rand
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		96.9374 	9717 		307 				257 				114 			143 
		100		98.5335 	9877 		147 				47 					1 				46 
		150 	98.7031 	9894 		130 				0 					0 				0 
		200	 	98.7031 	9894 		130 				0 					0 				0 

algorithm:fifo
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		96.8176 	9705 		319 				269 				120 			149 
		100	 	98.4238 	9866 		158 				58 					0 				58 
		150 	98.7031 	9894 		130 				0 					0 				0 
		200 	98.7031 	9894 		130 				0 					0 				0 

algorithm:lru
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50	 	97.7654 	9800  		224 	 			174  				58 				116 
 	 	100	 	98.6632 	9890  		134   	 	 		34 	 	 	 		0  	 		 	34 
 	 	150	 	98.7031 	9894 	 	130  		 	 	0 	 		  	 	0  	 	 	 	0 
 		200 	98.7031  	9894 	 	130  	 		 	0  		 	 	 	0  	 	 		0 

algorithm:clock
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
		50		97.7554 	9799 		225 				175 				55 				120 
		100		98.6233 	9886 		138 				38 					0 				38 
		150		98.7031 	9894 		130 				0 					0 				0 
		200		98.7031 	9894 		130 				0 					0 				0 


algorithm:opt
memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions
 		50		98.4637 	9870 		154 				104 				12 				92 
 		100 	98.7031 	9894 		130 				30 					0 				30 
 		150 	98.7031 	9894 		130 				0 					0 				0 
 		200 	98.7031 	9894 		130 				0 					0 				0 