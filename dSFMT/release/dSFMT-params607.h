#define POS1	1
#define SL1	15
#define SL2	5
#define SR1	5
#define SR2	16
#define MSK1	UINT64_C(0xfffbf7ffffffffff)
#define MSK2	UINT64_C(0xffb9ffffbfdbfffb)
#define MSK32_1	0xfffbf7ffU
#define MSK32_2	0xffffffffU
#define MSK32_3	0xffb9ffffU
#define MSK32_4	0xbfdbfffbU
#define PCV1	UINT64_C(0x0000000000000001)
#define PCV2	UINT64_C(0x0003b44600000000)
#define IDSTR	"dSFMT-607:1-15-5-5-16:fffbf7ffffffffff-ffb9ffffbfdbfffb"


/* PARAMETERS FOR ALTIVEC */
#define ALTI_SL1 7
#define SL1_PERM \
(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
#define SL1_MSK \
(vector unsigned int)(0xffffffffU,0xffff8000U,0xffffffffU,0xffff8000U)
#define SL2_PERM \
(vector unsigned char)(5,6,7,27,27,27,27,27,13,14,15,0,1,2,3,4)
#define ALTI_SR1 5
#define SR1_MSK \
(vector unsigned int)(0x07fbf7ffU,0xffffffffU,0x07b9ffffU,0xbfdbfffbU)
#define SR2_PERM \
(vector unsigned char)(18,18,0,1,2,3,4,5,18,18,8,9,10,11,12,13)
#define ALTI_PERM (vector unsigned char) \
  (8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7)
#define ALTI_LOW_MSK (vector unsigned int) \
  (LOW_MASK32_1, LOW_MASK32_2, LOW_MASK32_1, LOW_MASK32_2)
#define ALTI_HIGH_CONST (vector unsigned int)(HIGH_CONST32, 0, HIGH_CONST32, 0)
