//#define DEBUG_HT1
#ifdef  DEBUG_HT1
//#define PRINT_HT1_REG
//#define DEBUG_HT1_PARAM
#endif
#define CHECK_HT_PLL_MODE
#define CHECK_HT_PLL_LOCK
#define CHECK_7A_HT_PLL_LOCK

#ifdef  MULTI_CHIP
#define LS7A_2WAY_CONNECT
#endif

#define HT1_RECONNECT   1
//HT GEN1.0/3.0 cfg
#define HT1_GEN_CFG     3
//HT1 width cfg
#if defined(LS7A_2WAY_CONNECT) || (HT1_GEN_CFG == 1)
#define HT1_WIDTH_CFG   HT_WIDTH_CTRL_8BIT  //only support 8 bit
#else
#define HT1_WIDTH_CFG   HT_WIDTH_CTRL_16BIT
#endif
//HT1 freq cfg
#if (HT1_GEN_CFG == 3) 
#define HT1_HARD_FREQ_CFG       HT_GEN3_FREQ_CTRL_1600M
#define LS7A_HT1_SOFT_FREQ_CFG  (LS7A_HT_PLL_1600M | (0x1 << 1))
#define LS7A_HT1_SOFT_FREQ_CFG_C  (LS7A_C_HT_PLL_1600M | (0x1 << 1))
#define LS3A_HT1_SOFT_FREQ_CFG  (LS3A_HT_PLL_1600M | (0x1 << 1))
#else
#define HT1_HARD_FREQ_CFG       HT_GEN1_FREQ_CTRL_800M
//in HT GEN1 mode, define PLL freq to request freq x 2, for example, if you want to use HT1 800M, define HT_PLL_1600M
#define LS7A_HT1_SOFT_FREQ_CFG  (LS7A_HT_PLL_1600M | (0x1 << 1))
#define LS3A_HT1_SOFT_FREQ_CFG  (LS3A_HT_PLL_1600M | (0x1 << 1))
#endif

//if board use PCIE PortN but PRSNTnN is not correctly pull down or is connected to socket which will use a wider card than the socket,
//you need force enable PortN by define the macro bellow.
//N is 1 for PCIE F1/G0/G1/H and 1/2/3 for PCIE F0
//#define FORCE_ENABLE_PCIE_F0_P123
//#define FORCE_ENABLE_PCIE_F1_P1
//#define FORCE_ENABLE_PCIE_G0_P1
//#define FORCE_ENABLE_PCIE_G1_P1
//#define FORCE_ENABLE_PCIE_H_P1

#define LS7A_GRAPHICS_DISABLE   0

//#define LS7A_PCIE_NO_POWERDOWN
//staticly disable some PCIE Ports, no matter whether there is device
#define LS7A_PCIE_F0_DISABLE    0
#define LS7A_PCIE_F1_DISABLE    0
#define LS7A_PCIE_H_DISABLE     0
#define LS7A_PCIE_G0_DISABLE    0
#define LS7A_PCIE_G1_DISABLE    0

#define LS7A_SATA0_DISABLE  0
#define LS7A_SATA1_DISABLE  (LS7A_SATA0_DISABLE | 0)
#define LS7A_SATA2_DISABLE  (LS7A_SATA1_DISABLE | 0)
#define LS7A_USB0_DISABLE   0
#define LS7A_USB1_DISABLE   0
#define LS7A_GMAC0_DISABLE  0
#define LS7A_GMAC1_DISABLE  (LS7A_GMAC0_DISABLE | 0)
#define LS7A_LPC_DISABLE    0

//#define USE_PCIE_PAD_REFCLK
//#define USE_SATA_PAD_REFCLK
#define USE_USB_SYS_REFCLK

#if (!LS7A_GRAPHICS_DISABLE)
#define LS7A_GMEM_CFG
#endif
#ifdef  LS7A_GMEM_CFG
//#define DEBUG_GMEM_PARAM
//#define DEBUG_GMEM
#endif
