/*
 * Copyright 2018, DornerWorks
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DORNERWORKS_BSD)
 */

#ifndef VMLINUX_IMX8_H
#define VMLINUX_IMX8_H

#include <sel4arm-vmm/vm.h>

/* Copied from kernel/include/plat/zynqmp/plat/machine.h */

#define INTERRUPT_PPI_ID22                      22 /* Debug Communications Channel (DCC) */
#define INTERRUPT_PPI_ID23                      23 /* Performance Counter (PMU) overflow */
#define INTERRUPT_PPI_ID24                      24 /* Cross Trigger (CTI) */
#define INTERRUPT_PPI_ID25                      25 /* Virtual CPU interface maintenance*/
#define INTERRUPT_PPI_ID26                      26 /* hypervisor timer */
#define INTERRUPT_PPI_ID27                      27 /* virtual timer */
#define INTERRUPT_PPI_ID28                      28 /* PPI ID28 */
#define INTERRUPT_PPI_ID29                      29 /* PPI ID29 */
#define INTERRUPT_PPI_ID30                      30 /* Non-secure physical timer */
#define INTERRUPT_PPI_ID31                      31 /* PPI ID31 */
    /* Shared Peripheral Interrupt Mapping */
#define INTERRUPT_A53_nEXTERR                   32
#define INTERRUPT_A53_nINTERR                   33
#define INTERRUPT_A72_nEXTERR                   34
#define INTERRUPT_A72_nINTERR                   35
#define INTERRUPT_VPU_nEXTERR                   36
    //    INTERRUPT_RESERVED                    37
    //    INTERRUPT_RESERVED                    38
    //    INTERRUPT_RESERVED                    39
#define INTERRUPT_CCI_nERROR                    40
#define INTERRUPT_CCI_nEVNTCNTOVERFLOW_0        41
#define INTERRUPT_CCI_nEVNTCNTOVERFLOW_1        42
#define INTERRUPT_CCI_nEVNTCNTOVERFLOW_2        43
#define INTERRUPT_CCI_nEVNTCNTOVERFLOW_3        44
#define INTERRUPT_CCI_nEVNTCNTOVERFLOW_4        45
    //    INTERRUPT_RESERVED                    46
    //    INTERRUPT_RESERVED                    47
#define INTERRUPT_M40_INT_OUT_0                 48
#define INTERRUPT_M40_INT_OUT_1                 49
#define INTERRUPT_M40_INT_OUT_2                 50
#define INTERRUPT_M40_INT_OUT_3                 51
#define INTERRUPT_M40_INT_OUT_4                 52
#define INTERRUPT_M40_INT_OUT_5                 53
#define INTERRUPT_M40_INT_OUT_6                 54
#define INTERRUPT_M40_INT_OUT_7                 55
#define INTERRUPT_M41_INT_OUT_0                 56
#define INTERRUPT_M41_INT_OUT_1                 57
#define INTERRUPT_M41_INT_OUT_2                 58
#define INTERRUPT_M41_INT_OUT_3                 59
#define INTERRUPT_M41_INT_OUT_4                 60
#define INTERRUPT_M41_INT_OUT_5                 61
#define INTERRUPT_M41_INT_OUT_6                 62
#define INTERRUPT_M41_INT_OUT_7                 63
#define INTERRUPT_DBLog_comb_irpt_ns            64
#define INTERRUPT_DBLog_comb_irpt_s             65
#define INTERRUPT_DBLog_gbl_flt_irpt_ns         66
#define INTERRUPT_DBLog_gbl_flt_irpt_s          67
#define INTERRUPT_DBLog_perf_irpt_imx8_0        68
#define INTERRUPT_DBLog_perf_irpt_imx8_1        69
#define INTERRUPT_DBLog_perf_irpt_imx8_2        70
#define INTERRUPT_DBLog_perf_irpt_imx8_3        71
#define INTERRUPT_DISP_CTRLR0_INT_OUT_0         72
#define INTERRUPT_DISP_CTRLR0_INT_OUT_1         73
#define INTERRUPT_DISP_CTRLR0_INT_OUT_2         74
#define INTERRUPT_DISP_CTRLR0_INT_OUT_3         75
#define INTERRUPT_DISP_CTRLR0_INT_OUT_4         76
#define INTERRUPT_DISP_CTRLR0_INT_OUT_5         77
#define INTERRUPT_DISP_CTRLR0_INT_OUT_6         78
#define INTERRUPT_DISP_CTRLR0_INT_OUT_7         79
    // INTERRUPT_RESERVED                       80
#define INTERRUPT_DISP_CNTRLR0_INT_OUT_9        81
#define INTERRUPT_DISP_CNTRLR0_INT_OUT_10       82
#define INTERRUPT_DISP_CNTRLR0_INT_OUT_11       83
#define INTERRUPT_DISP_CNTRLR0_INT_OUT_12       84
    // INTERRUPT_RESERVED                       85
    // INTERRUPT_RESERVED                       86
    // INTERRUPT_RESERVED                       87
    // INTERRUPT_RESERVED                       88
#define INTERRUPT_LDVS_0_INT_OUT                89
#define INTERRUPT_LDVS_1_INT_OUT                90
#define INTERRUPT_MIPI_DSI_0_INT_OUT            91
#define INTERRUPT_MIPI_DSI_1_INT_OUT            92
#define INTERRUPT_HDMI_TX_INT_OUT               93
    //    INTERRUPT_RESERVED                    94
    //    INTERRUPT_RESERVED                    95
#define INTERRUPT_GPU0_xaq2_intr                96
#define INTERRUPT_GPU1_xaq2_intr                97
#define INTERRUPT_DMA_eDMA0_INT                 98
#define INTERRUPT_DMA_eDMA0_ERR_INT             99
#define INTERRUPT_DMA_eDMA1_INT                 100
#define INTERRUPT_DMA_eDMA1_ERR_INT             101
#define INTERRUPT_PCIeA_MSI_CTRL                102
#define INTERRUPT_PCIeA_CLK_REQ                 103
#define INTERRUPT_PCIeA_DMA                     104
#define INTERRUPT_PCIeA_INT_D                   105
#define INTERRUPT_PCIeA_INT_C                   106
#define INTERRUPT_PCIeA_INT_B                   107
#define INTERRUPT_PCIeA_INT_A                   108
#define INTERRUPT_PCIeA_SMLH_REQ_RST            109
#define INTERRUPT_PCIeA_GPIO_WAKEUP_0           110
#define INTERRUPT_PCIeA_GPIO_WAKEUP_1           111
#define INTERRUPT_LS_GPT0                       112
#define INTERRUPT_LS_GPT1                       113
#define INTERRUPT_LS_GPT2                       114
#define INTERRUPT_LS_GPT3                       115
#define INTERRUPT_LS_GPT4                       116
#define INTERRUPT_LS_KPP                        117
    //    INTERRUPT_RESERVED                    118
    //    INTERRUPT_RESERVED                    119
#define INTERRUPT_SATA_0                        120
#define INTERRUPT_SATA_1                        121
    //    INTERRUPT_RESERVED                    122
    //    INTERRUPT_RESERVED                    123
#define INTERRUPT_LSIO_OctaSPI0                 124
#define INTERRUPT_LSIO_OctaSPI1                 125
#define INTERRUPT_LSIO_PWM0                     126
#define INTERRUPT_LSIO_PWM1                     127
#define INTERRUPT_LSIO_PWM2                     128
#define INTERRUPT_LSIO_PWM3                     129
#define INTERRUPT_LSIO_PWM4                     130
#define INTERRUPT_LSIO_PWM5                     131
#define INTERRUPT_LSIO_PWM6                     132
#define INTERRUPT_LSIO_PWM7                     133
#define INTERRUPT_PCIeB_MSI_CTRL                134
#define INTERRUPT_PCIeB_CLK_REQ                 135
#define INTERRUPT_PCIeB_DMA                     136
#define INTERRUPT_PCIeB_INT_D                   137
#define INTERRUPT_PCIeB_INT_C                   138
#define INTERRUPT_PCIeB_INT_B                   139
#define INTERRUPT_PCIeB_INT_A                   140
#define INTERRUPT_PCIeB_INT_SMLH_REQ_RST        141
#define INTERRUPT_PCIeB_INT_GPIO_WAKEUP0        142
#define INTERRUPT_PCIeB_INT_GPIO_WAKEUP1        143
#define INTERRUPT_SCU_INT_OUT_0                 144
#define INTERRUPT_SCU_INT_OUT_1                 145
#define INTERRUPT_SCU_INT_OUT_2                 146
#define INTERRUPT_SCU_INT_OUT_3                 147
#define INTERRUPT_SCU_INT_OUT_4                 148
#define INTERRUPT_SCU_INT_OUT_5                 149
#define INTERRUPT_SCU_INT_OUT_6                 150
#define INTERRUPT_SCU_INT_OUT_7                 151
#define INTERRUPT_SCU_SYS_COUNT_INT_0           152
#define INTERRUPT_SCU_SYS_COUNT_INT_1           153
#define INTERRUPT_SCU_SYS_COUNT_INT_2           154
#define INTERRUPT_SCU_SYS_COUNT_INT_3           155
    //    INTERRUPT_RESERVED                    156
    //    INTERRUPT_RESERVED                    157
    //    INTERRUPT_RESERVED                    158
    //    INTERRUPT_RESERVED                    159
#define INTERRUPT_DRC0_DFI_ALERT_ERR            160
#define INTERRUPT_DRC1_DFI_ALERT_ERR            161
#define INTERRUPT_DRC0_PERF_CNT_FULL            162
#define INTERRUPT_DRC1_PERF_CNT_FULL            163
    //    INTERRUPT_RESERVED                    164
    //    INTERRUPT_RESERVED                    165
    //    INTERRUPT_RESERVED                    166
    //    INTERRUPT_RESERVED                    167
#define INTERRUPT_GPIO_INT_0                    168
#define INTERRUPT_GPIO_INT_1                    169
#define INTERRUPT_GPIO_INT_2                    170
#define INTERRUPT_GPIO_INT_3                    171
#define INTERRUPT_GPIO_INT_4                    172
#define INTERRUPT_GPIO_INT_5                    173
#define INTERRUPT_GPIO_INT_6                    174
#define INTERRUPT_GPIO_INT_7                    175
    //    INTERRUPT_RESERVED                    176
    //    INTERRUPT_RESERVED                    177
    //    INTERRUPT_RESERVED                    178
    //    INTERRUPT_RESERVED                    179
    //    INTERRUPT_RESERVED                    180
    //    INTERRUPT_RESERVED                    181
    //    INTERRUPT_RESERVED                    182
    //    INTERRUPT_RESERVED                    183
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_0        184
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_1        185
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_2        186
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_3        187
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_4        188
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_5        189
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_6        190
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_7        191
    //    INTERRUPT_DISP_CNTRLR1_INT_OUT_8      192
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_9        193
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_10       194
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_11       195
#define INTERRUPT_DISP_CNTRLR1_INT_OUT_12       196
    //    INTERRUPT_RESERVED                    197
    //    INTERRUPT_RESERVED                    198
    //    INTERRUPT_RESERVED                    199
#define INTERRUPT_VPU_SYS_INT_0                 200
#define INTERRUPT_VPU_SYS_INT_1                 201
    //    INTERRUPT_RESERVED                    202
    //    INTERRUPT_RESERVED                    203
    //    INTERRUPT_RESERVED                    204
    //    INTERRUPT_RESERVED                    205
    //    INTERRUPT_RESERVED                    206
    //    INTERRUPT_RESERVED                    207
#define INTERRUPT_LS_MU0                        208
#define INTERRUPT_LS_MU1                        209
#define INTERRUPT_LS_MU2                        210
#define INTERRUPT_LS_MU3                        211
#define INTERRUPT_LS_MU4                        212
    //    INTERRUPT_RESERVED                    213
    //    INTERRUPT_RESERVED                    214
    //    INTERRUPT_RESERVED                    215
#define INTERRUPT_LS_MU5_A                      216
#define INTERRUPT_LS_MU6_A                      217
#define INTERRUPT_LS_MU7_A                      218
#define INTERRUPT_LS_MU8_A                      219
#define INTERRUPT_LS_MU9_A                      220
#define INTERRUPT_LS_MU10_A                     221
#define INTERRUPT_LS_MU11_A                     222
#define INTERRUPT_LS_MU12_A                     223
#define INTERRUPT_LS_MU13_A                     224
    //    INTERRUPT_RESERVED                    225
    //    INTERRUPT_RESERVED                    226
    //    INTERRUPT_RESERVED                    227
    //    INTERRUPT_RESERVED                    228
    //    INTERRUPT_RESERVED                    229
    //    INTERRUPT_RESERVED                    230
    //    INTERRUPT_RESERVED                    231
#define INTERRUPT_LS_MU5_B                      232
#define INTERRUPT_LS_MU6_B                      233
#define INTERRUPT_LS_MU7_B                      234
#define INTERRUPT_LS_MU8_B                      235
#define INTERRUPT_LS_MU9_B                      236
#define INTERRUPT_LS_MU10_B                     237
#define INTERRUPT_LS_MU11_B                     238
#define INTERRUPT_LS_MU12_B                     239
#define INTERRUPT_LS_MU13_B                     240
    //    INTERRUPT_RESERVED                    241
    //    INTERRUPT_RESERVED                    242
    //    INTERRUPT_RESERVED                    243
    //    INTERRUPT_RESERVED                    244
    //    INTERRUPT_RESERVED                    245
    //    INTERRUPT_RESERVED                    246
    //    INTERRUPT_RESERVED                    247
#define INTERRUPT_DMA_SPI0                      248
#define INTERRUPT_DMA_SPI1                      249
#define INTERRUPT_DMA_SPI2                      250
#define INTERRUPT_DMA_SPI3                      251
#define INTERRUPT_DMA_I2C0                      252
#define INTERRUPT_DMA_I2C1                      253
#define INTERRUPT_DMA_I2C2                      254
#define INTERRUPT_DMA_I2C3                      255
#define INTERRUPT_DMA_I2C4                      256
#define INTERRUPT_DMA_UART0                     257
#define INTERRUPT_DMA_UART1                     258
#define INTERRUPT_DMA_UART2                     259
#define INTERRUPT_DMA_UART3                     260
#define INTERRUPT_DMA_UART4                     261
#define INTERRUPT_DMA_SIM0                      262
#define INTERRUPT_DMA_SIM1                      263
#define INTERRUPT_uSDHC0                        264
#define INTERRUPT_uSDHC1                        265
#define INTERRUPT_uSDHC2                        266
#define INTERRUPT_DMA_FlexCAN0                  267
#define INTERRUPT_DMA_FlexCAN1                  268
#define INTERRUPT_DMA_FlexCAN2                  269
#define INTERRUPT_DMA_FTM0                      270
#define INTERRUPT_DMA_FTM1                      271
#define INTERRUPT_DMA_ADC0                      272
#define INTERRUPT_DMA_ADC1                      273
#define INTERRUPT_DMA_EXTERNAL_0                274
#define INTERRUPT_DMA_EXTERNAL_1                275
#define INTERRUPT_DMA_EXTERNAL_2                276
#define INTERRUPT_DMA_EXTERNAL_3                277
#define INTERRUPT_DMA_EXTERNAL_4                278
#define INTERRUPT_DMA_EXTERNAL_5                279
    //    INTERRUPT_RESERVED                    280
    //    INTERRUPT_RESERVED                    281
    //    INTERRUPT_RESERVED                    282
    //    INTERRUPT_RESERVED                    283
    //    INTERRUPT_RESERVED                    284
    //    INTERRUPT_RESERVED                    285
    //    INTERRUPT_RESERVED                    286
    //    INTERRUPT_RESERVED                    287
#define INTERRUPT_ENET0_FRAME1                  288
#define INTERRUPT_ENET0_FRAME2                  289
#define INTERRUPT_ENET0_FRAME0_EVENT            290
#define INTERRUPT_ENET0_TIMER                   291
#define INTERRUPT_ENET1_FRAME1                  292
#define INTERRUPT_ENET1_FRAME2                  293
#define INTERRUPT_ENET1_FRAME0_EVENT            294
#define INTERRUPT_ENET1_TIMER                   295
#define INTERRUPT_DTCP                          296
#define INTERRUPT_MLB                           297
#define INTERRUPT_MLB_AHB                       298
#define INTERRUPT_USB_OTG                       299
#define INTERRUPT_USB_HOST                      300
#define INTERRUPT_UTMI                          301
#define INTERRUPT_WAKEUP                        302
#define INTERRUPT_USB3                          303
#define INTERRUPT_ND_FLASH_BCH                  304
#define INTERRUPT_ND_FLASH_GPMI                 305
#define INTERRUPT_APBHDMA                       306
#define INTERRUPT_DMA                           307
#define INTERRUPT_DMA_ERR                       308
    //    INTERRUPT_RESERVED                    309
    //    INTERRUPT_RESERVED                    310
    //    INTERRUPT_RESERVED                    311
    //    INTERRUPT_RESERVED                    312
    //    INTERRUPT_RESERVED                    313
    //    INTERRUPT_RESERVED                    314
    //    INTERRUPT_RESERVED                    315
    //    INTERRUPT_RESERVED                    316
    //    INTERRUPT_RESERVED                    317
    //    INTERRUPT_RESERVED                    318
    //    INTERRUPT_RESERVED                    319
#define INTERRUPT_MSI                           320
    //    INTERRUPT_RESERVED                    321
    //    INTERRUPT_RESERVED                    322
    //    INTERRUPT_RESERVED                    323
    //    INTERRUPT_RESERVED                    324
    //    INTERRUPT_RESERVED                    325
    //    INTERRUPT_RESERVED                    326
    //    INTERRUPT_RESERVED                    327
    //    INTERRUPT_RESERVED                    328
#define INTERRUPT_PDMA_STREAM0                  329
#define INTERRUPT_PDMA_STREAM1                  330
#define INTERRUPT_PDMA_STREAM2                  331
#define INTERRUPT_PDMA_STREAM3                  332
#define INTERRUPT_PDMA_STREAM4                  333
#define INTERRUPT_PDMA_STREAM5                  334
#define INTERRUPT_PDMA_STREAM6                  335
#define INTERRUPT_PDMA_STREAM7                  336
#define INTERRUPT_MJPEG_ENC0                    337
#define INTERRUPT_MJPEG_ENC1                    338
#define INTERRUPT_MJPEG_ENC2                    339
#define INTERRUPT_MJPEG_ENC3                    340
#define INTERRUPT_MJPEG_DEC0                    341
#define INTERRUPT_MJPEG_DEC1                    342
#define INTERRUPT_MJPEG_DEC2                    343
#define INTERRUPT_MJPEG_DEC3                    344
    //    INTERRUPT_RESERVED                    345
#define INTERRUPT_SAI0_MOD                      346
#define INTERRUPT_SAI0_DMA                      347
#define INTERRUPT_SAI1_MOD                      348
#define INTERRUPT_SAI1_DMA                      349
#define INTERRUPT_SAI2_MOD                      350
#define INTERRUPT_SAI2_DMA                      351
#define INTERRUPT_MIPI_CSI0_OUT                 352
#define INTERRUPT_MIPI_CSI1_OUT                 353
#define INTERRUPT_HDMI_RX_OUT                   354
#define INTERRUPT_SAI3_MOD                      355
#define INTERRUPT_SAI3_DMA                      356
#define INTERRUPT_SAI_HDMI_RX_MOD               357
#define INTERRUPT_SAI_HDMI_RX_DMA               358
#define INTERRUPT_SAI_HDMI_TX_MOD               359
#define INTERRUPT_SAI_HDMI_TX_DMA               360
#define INTERRUPT_SAI6_MOD                      361
#define INTERRUPT_SAI6_DMA                      362
#define INTERRUPT_SAI7_MOD                      363
#define INTERRUPT_SAI7_DMA                      364
    //    INTERRUPT_RESERVED                    365
    //    INTERRUPT_RESERVED                    366
    //    INTERRUPT_RESERVED                    367
#define INTERRUPT_SPI0_MOD                      368
#define INTERRUPT_SPI1_MOD                      369
#define INTERRUPT_SPI2_MOD                      370
#define INTERRUPT_SPI3_MOD                      371
#define INTERRUPT_I2C0                          372
#define INTERRUPT_I2C1                          373
#define INTERRUPT_I2C2                          374
#define INTERRUPT_I2C3                          375
#define INTERRUPT_I2C4                          376
#define INTERRUPT_UART0_MOD                     377
#define INTERRUPT_UART1_MOD                     378
#define INTERRUPT_UART2_MOD                     379
#define INTERRUPT_UART3_MOD                     380
#define INTERRUPT_UART4_MOD                     381
#define INTERRUPT_SIM0_MOD                      382
#define INTERRUPT_SIM1_MOD                      383
#define INTERRUPT_FlexCAN0_MOD                  384
#define INTERRUPT_FlexCAN1_MOD                  385
#define INTERRUPT_FlexCAN2_MOD                  386
#define INTERRUPT_FTM0_MOD                      387
#define INTERRUPT_FTM1_MOD                      388
#define INTERRUPT_ADC0_MOD                      389
#define INTERRUPT_ADC1_MOD                      390
#define INTERRUPT_FlexCAN0_DMA                  391
#define INTERRUPT_FlexCAN1_DMA                  392
#define INTERRUPT_FlexCAN2_DMA                  393
#define INTERRUPT_FTM0_DMA                      394
#define INTERRUPT_FTM1_DMA                      395
#define INTERRUPT_ADC0_DMA                      396
#define INTERRUPT_ADC1_DMA                      397
    //    INTERRUPT_RESERVED                    398
    //    INTERRUPT_RESERVED                    399
#define INTERRUPT_eDMA0                         400
#define INTERRUPT_eDMA0_ERR                     401
#define INTERRUPT_eDMA1                         402
#define INTERRUPT_eDMA1_ERR                     403
#define INTERRUPT_ASRC0_INT1                    404
#define INTERRUPT_ASRC0_INT2                    405
#define INTERRUPT_DMA0_CH0                      406
#define INTERRUPT_DMA0_CH1                      407
#define INTERRUPT_DMA0_CH2                      408
#define INTERRUPT_DMA0_CH3                      409
#define INTERRUPT_DMA0_CH4                      410
#define INTERRUPT_DMA0_CH5                      411
#define INTERRUPT_ASRC1_INT1                    412
#define INTERRUPT_ASRC1_INT2                    413
#define INTERRUPT_DMA1_CH0                      414
#define INTERRUPT_DMA1_CH1                      415
#define INTERRUPT_DMA1_CH2                      416
#define INTERRUPT_DMA1_CH3                      417
#define INTERRUPT_DMA1_CH4                      418
#define INTERRUPT_DMA1_CH5                      419
#define INTERRUPT_ESAI0                         420
#define INTERRUPT_ESAI1                         421
    //    INTERRUPT_UNUSED                      422
#define INTERRUPT_GPT0                          423
#define INTERRUPT_GPT1                          424
#define INTERRUPT_GPT2                          425
#define INTERRUPT_GPT3                          426
#define INTERRUPT_GPT4                          427
#define INTERRUPT_GPT5                          428
#define INTERRUPT_SAI0                          429
#define INTERRUPT_SAI1                          430
#define INTERRUPT_SAI2                          431
#define INTERRUPT_SAI3                          432
#define INTERRUPT_SAI_HDMI_RX                   433
#define INTERRUPT_SAI_HDMI_TX                   434
#define INTERRUPT_SAI6                          435
#define INTERRUPT_SAI7                          436
#define INTERRUPT_SPDIF0_RX                     437
#define INTERRUPT_SPDIF0_TX                     438
#define INTERRUPT_SPDIF1_RX                     439
#define INTERRUPT_SPDIF1_TX                     440
#define INTERRUPT_ESAI0_MOD                     441
#define INTERRUPT_ESAI0_DMA                     442
#define INTERRUPT_ESAI1_MOD                     443
#define INTERRUPT_ESAI1_DMA                     444
    //    INTERRUPT_RESERVED                    445
    //    INTERRUPT_RESERVED                    446
    //    INTERRUPT_RESERVED                    447
#define INTERRUPT_DMA_SPI0_RX                   448
#define INTERRUPT_DMA_SPI0_TX                   449
#define INTERRUPT_DMA_SPI1_RX                   450
#define INTERRUPT_DMA_SPI1_TX                   451
#define INTERRUPT_DMA_SPI2_RX                   452
#define INTERRUPT_DMA_SPI2_TX                   453
#define INTERRUPT_DMA_SPI3_RX                   454
#define INTERRUPT_DMA_SPI3_TX                   455
#define INTERRUPT_DMA_I2C0_RX                   456
#define INTERRUPT_DMA_I2C0_TX                   457
#define INTERRUPT_DMA_I2C1_RX                   458
#define INTERRUPT_DMA_I2C1_TX                   459
#define INTERRUPT_DMA_I2C2_RX                   460
#define INTERRUPT_DMA_I2C2_TX                   461
#define INTERRUPT_DMA_I2C3_RX                   462
#define INTERRUPT_DMA_I2C3_TX                   463
#define INTERRUPT_DMA_I2C4_RX                   464
#define INTERRUPT_DMA_I2C4_TX                   465
#define INTERRUPT_UART0_RX                      466
#define INTERRUPT_UART0_TX                      467
#define INTERRUPT_UART1_RX                      468
#define INTERRUPT_UART1_TX                      469
#define INTERRUPT_UART2_RX                      470
#define INTERRUPT_UART2_TX                      471
#define INTERRUPT_UART3_RX                      472
#define INTERRUPT_UART3_TX                      473
#define INTERRUPT_UART4_RX                      474
#define INTERRUPT_UART4_TX                      475
#define INTERRUPT_SIM0_RX                       476
#define INTERRUPT_SIM0_TX                       477
#define INTERRUPT_SIM1_RX                       478
#define INTERRUPT_SIM1_TX                       479
#define INTERRUPT_SEC_MU1_A                     480
#define INTERRUPT_SEC_MU2_A                     481
#define INTERRUPT_SEC_MU3_A                     482
#define INTERRUPT_CAAM_INT0                     483
#define INTERRUPT_CAAM_INT1                     484
#define INTERRUPT_CAAM_INT2                     485
#define INTERRUPT_CAAM_INT3                     486
#define INTERRUPT_CAAM_RTIC                     487
#define INTERRUPT_SPDIF0_RX_MOD                 488
#define INTERRUPT_SPDIF0_RX_DMA                 489
#define INTERRUPT_SPDIF0_TX_MOD                 490
#define INTERRUPT_SPDIF0_TX_DMA                 491
#define INTERRUPT_SPDIF1_RX_MOD                 492
#define INTERRUPT_SPDIF1_RX_DMA                 493
#define INTERRUPT_SPDIF1_TX_MOD                 494
#define INTERRUPT_SPDIF1_TX_DMA                 495
#define INTERRUPT_VPU_INT_0                     496
#define INTERRUPT_VPU_INT_1                     497
#define INTERRUPT_VPU_INT_2                     498
#define INTERRUPT_VPU_INT_3                     499
#define INTERRUPT_VPU_INT_4                     500
#define INTERRUPT_VPU_INT_5                     501
#define INTERRUPT_VPU_INT_6                     502
#define INTERRUPT_VPU_INT_7                     503

#define MAX_IRQ                                 512

#define LINUX_RAM_BASE       0x900000000
#define LINUX_RAM_END        0x980000000
#define LINUX_RAM_SIZE       LINUX_RAM_END - LINUX_RAM_BASE
#define LINUX_RAM_OFFSET     0

#define INTERRUPT_CORE_VIRT_TIMER INTERRUPT_PPI_ID27

/* The following #defines are configuration data that may need to change
 * based on specific configurations
 */

#define NUM_VMS     3
#define DMA_VSTART  0x920000000

#define VUART_ENABLED
#define VUART_IRQ   510

#endif /* VMLINUX_H */
