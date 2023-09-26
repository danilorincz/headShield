# Sample raw data as a string
raw_data = """#1
76
73
3539
3530
4038
#2
76
73
3541
3530
3547
#3
76
73
3537
3529
3545
#4
75
73
3534
3526
3540
#5
75
73
3536
3529
3540
#6
75
73
3539
3531
3541
#7
75
73
3538
3533
3542
#8
74
73
3538
3534
3543
#9
75
73
3539
3533
3545
#10
74
73
3540
3531
3543
#11
74
73
3538
3533
3544
#12
73
73
3537
3533
3541
#13
73
73
3536
3531
3541
#14
73
73
3534
3531
3540
#15
73
72
3537
3531
3540
#16
72
72
3536
3530
3540
#17
72
72
3534
3529
3539
#18
72
72
3534
3530
3538
#19
72
71
3535
3529
3537
#20
71
71
3535
3530
3540
#21
71
70
3534
3530
3539
#22
70
70
3534
3530
3537
#23
70
70
3534
3530
3538
#24
70
70
3539
3531
3540
#25
70
70
3534
3530
3540
#26
70
70
3531
3530
3540
#27
69
69
3535
3531
3539
#28
69
69
3533
3530
3540
#29
69
69
3533
3530
3538
#30
68
68
3535
3529
3538
#31
68
68
3532
3530
3539
#32
68
68
3536
3530
3538
#33
68
68
3533
3531
3539
#34
68
68
3533
3527
3538
#35
67
67
3536
3529
3539
#36
67
67
3535
3529
3539
#37
67
66
3534
3530
3538
#38
66
66
3534
3530
3539
#39
66
66
3535
3529
3538
#40
66
65
3535
3528
3539
#41
65
65
3535
3531
3539
#42
65
65
3537
3530
3538
#43
65
65
3536
3531
3541
#44
65
65
3536
3532
3540
#45
64
64
3533
3529
3540
#46
64
64
3533
3529
3538
#47
64
64
3535
3530
3538
#48
63
63
3534
3528
3538
#49
63
63
3536
3530
3539
#50
63
63
3534
3529
3539
#51
63
63
3534
3529
3538
#52
63
62
3534
3530
3538
#53
62
62
3533
3530
3540
#54
62
62
3536
3530
3538
#55
62
62
3535
3531
3539
#56
62
61
3534
3530
3540
#57
61
61
3534
3530
3539
#58
61
60
3540
3529
3540
#59
60
60
3534
3527
3540
#60
60
60
3535
3529
3540
#61
60
60
3536
3531
3540
#62
60
60
3535
3530
3538
#63
59
59
3530
3530
3539
#64
59
59
3534
3529
3540
#65
59
59
3534
3531
3540
#66
59
58
3535
3531
3540
#67
58
58
3533
3529
3542
#68
58
58
3533
3530
3540
#69
58
58
3535
3531
3541
#70
58
58
3534
3530
3540
#71
57
57
3535
3528
3539
#72
57
57
3537
3530
3540
#73
57
57
3539
3532
3541
#74
57
56
3537
3530
3542
#75
56
56
3538
3531
3540
#76
56
56
3539
3533
3542
#77
56
56
3537
3530
3542
#78
56
55
3538
3532
3542
#79
55
55
3537
3533
3541
#80
55
55
3536
3531
3540
#81
55
55
3536
3532
3543
#82
55
54
3537
3532
3543
#83
54
54
3536
3532
3543
#84
54
54
3535
3532
3541
#85
54
54
3538
3533
3542
#86
53
53
3538
3534
3545
#87
53
53
3538
3534
3543
#88
53
53
3537
3534
3541
#89
53
53
3537
3533
3542
#90
53
52
3536
3533
3542
#91
52
52
3538
3534
3542
#92
52
52
3537
3534
3542
#93
52
52
3539
3532
3542
#94
51
51
3539
3532
3542
#95
51
51
3538
3533
3542
#96
51
51
3537
3531
3540
#97
51
50
3534
3533
3541
#98
50
50
3538
3534
3541
#99
50
50
3537
3533
3543
#100
50
50
3538
3533
3542
#101
50
50
3538
3533
3542
#102
50
49
3538
3534
3542
#103
49
49
3538
3532
3541
#104
49
49
3537
3533
3542
#105
49
48
3536
3533
3541
#106
49
48
3536
3533
3542
#107
48
48
3540
3534
3542
#108
48
48
3537
3533
3542
#109
48
48
3539
3533
3542
#110
48
48
3537
3533
3542
#111
48
48
3539
3533
3541
#112
47
47
3539
3533
3542
#113
47
47
3536
3533
3543
#114
47
47
3537
3533
3541
#115
47
46
3536
3533
3541
#116
46
46
3534
3532
3541
#117
46
46
3537
3532
3542
#118
46
46
3539
3532
3542
#119
46
45
3537
3532
3540
#120
45
45
3540
3534
3542
#121
45
45
3537
3533
3541
#122
45
45
3537
3534
3540
#123
45
45
3539
3533
3541
#124
45
45
3534
3532
3541
#125
45
44
3540
3534
3542
#126
45
44
3538
3533
3541
#127
45
44
3536
3534
3541
#128
44
44
3539
3534
3543
#129
44
44
3537
3534
3542
#130
44
43
3536
3534
3541
#131
43
43
3540
3533
3542
#132
44
43
3538
3534
3542
#133
43
43
3536
3533
3542
#134
43
43
3537
3532
3542
#135
43
43
3539
3533
3543
#136
43
43
3537
3534
3543
#137
43
43
3539
3532
3542
#138
43
43
3535
3532
3542
#139
43
42
3535
3532
3541
#140
42
42
3538
3533
3542
#141
42
42
3541
3535
3542
#142
42
42
3536
3533
3544
#143
42
42
3539
3533
3542
#144
42
41
3538
3534
3544
#145
42
41
3538
3532
3541
#146
42
41
3540
3533
3542
#147
41
41
3539
3534
3544
#148
41
41
3540
3534
3543
#149
41
41
3540
3534
3545
#150
41
41
3538
3533
3544
#151
41
40
3536
3531
3544
#152
40
40
3538
3532
3544
#153
40
40
3538
3534
3545
#154
40
40
3537
3534
3544
#155
40
40
3540
3534
3545
#156
40
40
3538
3531
3543
#157
40
40
3536
3534
3542
#158
40
40
3536
3529
3544
#159
40
40
3536
3531
3542
#160
40
40
3537
3531
3541
#161
40
40
3539
3528
3542
#162
40
39
3537
3532
3543
#163
40
39
3535
3531
3543
#164
40
39
3535
3531
3543
#165
40
39
3537
3532
3542
#166
39
39
3541
3534
3543
#167
39
39
3538
3533
3543
#168
39
39
3538
3532
3543
#169
39
39
3537
3531
3542
#170
39
38
3535
3531
3541
#171
39
38
3535
3529
3542
#172
39
38
3539
3533
3543
#173
39
38
3540
3533
3543
#174
39
38
3537
3532
3543
#175
38
38
3535
3529
3541
#176
38
38
3534
3531
3541
#177
38
38
3535
3530
3544
#178
38
38
3537
3530
3539
#179
38
38
3538
3531
3541
#180
38
38
3536
3530
3540
#181
38
38
3539
3530
3540
#182
38
38
3535
3529
3540
#183
38
37
3535
3528
3540
#184
38
37
3531
3528
3540
#185
38
37
3537
3529
3539
#186
37
37
3538
3530
3542
#187
37
37
3536
3530
3541
#188
37
37
3534
3530
3541
#189
37
37
3537
3529
3541
#190
37
37
3535
3530
3539
#191
37
36
3538
3531
3542
#192
37
36
3535
3531
3540
#193
37
36
3533
3528
3539
#194
36
36
3535
3529
3542
#195
36
36
3537
3529
3540
#196
36
36
3534
3529
3538
#197
36
36
3532
3529
3539
#198
36
36
3535
3529
3540
#199
36
36
3533
3530
3540
#200
36
36
3537
3530
3540
#201
36
35
3531
3528
3540
#202
36
35
3531
3527
3541
#203
36
35
3538
3530
3540
#204
35
35
3533
3529
3539
#205
35
35
3531
3529
3539
#206
35
35
3536
3528
3539
#207
35
35
3535
3529
3541
#208
35
35
3536
3531
3541
#209
35
35
3532
3530
3542
#210
35
35
3533
3528
3539
#211
35
34
3537
3530
3542
#212
35
34
3535
3529
3541
#213
34
34
3535
3529
3540
#214
34
34
3536
3530
3541
#215
34
34
3532
3529
3542
#216
34
34
3534
3529
3541
#217
34
34
3531
3528
3539
#218
34
33
3538
3529
3542
#219
34
33
3537
3531
3542
#220
34
33
3536
3528
3542
#221
33
33
3535
3530
3540
#222
33
33
3536
3529
3540
#223
33
33
3538
3532
3543
#224
33
33
3536
3530
3542
#225
33
33
3534
3528
3542
#226
33
33
3535
3527
3539
#227
33
33
3533
3529
3540
#228
33
33
3530
3528
3540
#229
33
32
3537
3528
3540
#230
32
32
3535
3530
3542
#231
32
32
3532
3527
3543
#232
32
32
3533
3528
3539
#233
32
32
3538
3527
3539
#234
32
31
3533
3527
3540
#235
32
31
3537
3525
3542
#236
31
31
3535
3527
3540
#237
31
31
3533
3525
3541
#238
31
30
3536
3526
3539
#239
31
30
3536
3528
3541
#240
30
30
3536
3526
3542
#241
30
30
3534
3528
3542
#242
30
30
3534
3527
3538
#243
30
30
3531
3527
3540
#244
30
30
3534
3527
3542
#245
30
29
3535
3529
3542
#246
29
29
3536
3529
3542
#247
29
29
3534
3528
3540
#248
29
29
3538
3529
3542
#249
29
28
3539
3529
3544
#250
28
28
3531
3530
3542
#251
28
28
3538
3529
3544
#252
28
28
3535
3530
3543
#253
28
28
3537
3530
3541
#254
28
27
3533
3527
3541
#255
27
27
3537
3527
3540
#256
27
27
3534
3525
3541
#257
27
27
3534
3528
3539
#258
27
26
3535
3527
3541
#259
26
26
3540
3527
3543
#260
26
26
3534
3529
3540
#261
26
25
3535
3527
3542
#262
26
25
3537
3529
3541
#263
25
25
3537
3529
3541
#264
25
25
3531
3529
3541
#265
25
25
3534
3527
3542
#266
25
24
3537
3532
3542
#267
24
24
3538
3531
3542
#268
24
24
3538
3529
3542
#269
23
23
3532
3528
3540
#270
23
23
3538
3530
3543
#271
23
22
3537
3531
3543
#272
22
22
3539
3531
3543
#273
21
20
3536
3532
3543
#274
20
20
3537
3531
3542
#275
19
19
3540
3532
3548
#276
17
17
3538
3532
3543
#277
15
15
3536
3532
3545
#278
13
13
3539
3533
3545
#279
9
9
3540
3531
3545
#280
5
5
3540
3534
3543
"""

# Split the raw data into lines
lines = raw_data.strip().split("\n")

# Open a file to write the formatted data
with open("formatted_data.txt", "w") as f:
    # Write the header
    f.write("#\tBattery%\tLowest%\tTachoFinal\tMinTacho\tMaxTacho\n")
    
    # Initialize an empty list to hold the current test data
    current_test = []
    
    for line in lines:
        if line.startswith("#"):
            # If we encounter a new test and current_test is not empty, write it to the file
            if current_test:
                f.write("\t".join(current_test) + "\n")
            # Start a new test
            current_test = [line[1:]]
        else:
            # Append the data to the current test
            current_test.append(line)
    
    # Write the last test if it exists
    if current_test:
        f.write("\t".join(current_test) + "\n")

print("Formatted data has been written to 'formatted_data.txt'")