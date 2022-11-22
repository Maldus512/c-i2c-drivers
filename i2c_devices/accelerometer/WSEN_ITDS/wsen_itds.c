#include "wsen_itds.h"
#include "../../../i2c_common/i2c_common.h"
#include <stdint.h>
#include <stdlib.h>
#include <xc.h>

#define REG_CTRL1 0x20
#define REG_CTRL2 0x21
#define REG_CTRL3 0x22
#define REG_CTRL4 0x23
#define REG_CTRL5 0x24
#define REG_CTRL6 0x25

#define REG_STATUS 0x27
#define REG_X_OUT_L 0x28
#define REG_X_OUT_H 0x29
#define REG_Y_OUT_L 0x2A
#define REG_Y_OUT_H 0x2B
#define REG_Z_OUT_L 0x2C
#define REG_Z_OUT_H 0x2D
#define REG_DEVICE_ID 0x0F

#define REG_CTRL1_LP_MODE_MASK 0x3
#define REG_CTRL1_LP_MODE_SHIFT 0
#define REG_CTRL1_LP_MODE_LOW 0
#define REG_CTRL1_LP_MODE_NORMAL 2

#define REG_CTRL1_MODE_MASK 0x3
#define REG_CTRL1_MODE_SHIFT 2
#define REG_CTRL1_MODE_NORMAL 0
#define REG_CTRL1_MODE_HIGH_PERF 1
#define REG_CTRL1_MODE_SDC 2

#define REG_CTRL1_ORD_MASK 0xF0
#define REG_CTRL1_ORD_SHIFT 4
// REG_CTRL1_ORD_{high performance hz}_{normal mode hz}_{low power mode}
#define REG_CTRL1_ORD_12_5HZ_12_5HZ_1_6HZ 1
#define REG_CTRL1_ORD_12_5HZ_12_5HZ_12_5HZ 2
#define REG_CTRL1_ORD_25HZ_25HZ_25HZ 3
#define REG_CTRL1_ORD_50HZ_50HZ_50HZ 4
#define REG_CTRL1_ORD_100HZ_100HZ_100HZ 5
#define REG_CTRL1_ORD_200HZ_200HZ_200HZ 6
#define REG_CTRL1_ORD_400HZ_200HZ_200HZ 7
#define REG_CTRL1_ORD_800HZ_200HZ_200HZ 7
#define REG_CTRL1_ORD_1600HZ_200HZ_200HZ 7

#define REG_CTRL2_I2C_DISABLE 1
#define REG_CTRL2_IF_ADD_INC 2
#define REG_CTRL2_BDU 3
#define REG_CTRL2_CP_PU_DISC 4
#define REG_CTRL2_SOFT_RESET 6
#define REG_CTRL2_BOOT 7

#define REG_CTRL3_SLP_MODE_1 0
#define REG_CTRL3_SLP_MODE_SEL 1
#define REG_CTRL3_H_LACTIVE 3
#define REG_CTRL3_LIR 4
#define REG_CTRL3_PP_OD 5
#define REG_CTRL3_ST 6

#define REG_CTRL4_INT0_DRDY 0
#define REG_CTRL4_INT0_FTH 1
#define REG_CTRL4_INT0_DIFF5 2
#define REG_CTRL4_INT0_TAP 3
#define REG_CTRL4_INT0_FF 4
#define REG_CTRL4_INT0_WU 5
#define REG_CTRL4_INT0_SINGLE_TAP 6
#define REG_CTRL4_INT0_6D 7

#define REG_CTRL5_INT1_DRDY 0
#define REG_CTRL5_INT1_FTH 1
#define REG_CTRL5_INT1_DIFF5 2
#define REG_CTRL5_INT1_OVR 3
#define REG_CTRL5_INT1_DRDY_T 4
#define REG_CTRL5_INT1_BOOT 5
#define REG_CTRL5_INT1_SLEEP_CHG 6
#define REG_CTRL5_INT1_SLEEP_STATE 7

#define REG_CTRL6_LOW_NOISE 2
#define REG_CTRL6_FDS 3
#define REG_CTRL6_FS 4
#define REG_CTRL6_BW_FILT 5
#define REG_CTRL6_FS_MASK 0x30
#define REG_CTRL6_BW_FILT_MASK 0xC0
#define REG_CTRL6_BW_FILT_2 0
#define REG_CTRL6_BW_FILT_4 1
#define REG_CTRL6_BW_FILT_10 2
#define REG_CTRL6_BW_FILT_20 3
#define REG_CTRL6_FS_2 0
#define REG_CTRL6_FS_4 1
#define REG_CTRL6_FS_8 2
#define REG_CTRL6_FS_16 3

int wsen_itds_update_register(i2c_driver_t driver, uint8_t reg, uint8_t mask,
        uint8_t val) {
    int res;
    uint8_t old_val;
    res = i2c_read_register(driver, reg, &old_val, 1);
    if (res)
        return res;

    old_val &= ~mask;
    uint8_t new_val = old_val | val;
    res = i2c_write_register(driver, reg, &new_val, 1);

    return res;
}

int wsen_itds_update_ctrl1(i2c_driver_t driver, uint8_t reg, uint8_t shift,
        uint8_t mask) {
    int res;
    uint8_t val = reg << shift;
    val = res = wsen_itds_update_register(driver, REG_CTRL1, mask, val);
    return res;
}

int wsen_itds_set_normal_mode(i2c_driver_t driver) {
    return wsen_itds_update_ctrl1(driver, REG_CTRL1_MODE_NORMAL,
            REG_CTRL1_MODE_SHIFT, REG_CTRL1_MODE_MASK);
}

int wsen_itds_set_high_performance_mode(i2c_driver_t driver) {
    return wsen_itds_update_ctrl1(driver, REG_CTRL1_MODE_HIGH_PERF,
            REG_CTRL1_MODE_SHIFT, REG_CTRL1_MODE_MASK);
}

int wsen_itds_set_single_data_conv_mode(i2c_driver_t driver) {
    return wsen_itds_update_ctrl1(driver, REG_CTRL1_MODE_SDC,
            REG_CTRL1_MODE_SHIFT, REG_CTRL1_MODE_MASK);
}

int wsen_itds_set_ord(i2c_driver_t driver, uint8_t reg_val) {
    return wsen_itds_update_ctrl1(driver, reg_val, REG_CTRL1_ORD_SHIFT,
            REG_CTRL1_ORD_MASK);
}

int wsen_itds_update_ctrl2(i2c_driver_t driver, uint8_t reg, uint8_t val) {
    int res;
    uint8_t shifted_val = val << reg;
    uint8_t mask = 1 << reg;
    res = wsen_itds_update_register(driver, REG_CTRL2, mask, shifted_val);
    return res;
}

int wsen_itds_reboot(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_BOOT, val);
}

int wsen_itds_soft_reset(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_SOFT_RESET, val);
}

int wsen_itds_cp_pu_disc(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_CP_PU_DISC, val);
}

int wsen_itds_bdu(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_BDU, val);
}

int wsen_itds_if_add_inc(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_IF_ADD_INC, val);
}

int wsen_itds_i2c_disable(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, REG_CTRL2_I2C_DISABLE, val);
}

int wsen_itds_update_ctrl3(i2c_driver_t driver, uint8_t reg, uint8_t val) {
    int res;
    uint8_t shifted_val = val << reg;
    uint8_t mask = 1 << reg;
    if (reg == REG_CTRL3_ST)
        mask += 1 << (reg + 1);
    res = wsen_itds_update_register(driver, REG_CTRL3, 0, shifted_val);
    return res;
}

int wsen_itds_slp_mode_1(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_SLP_MODE_1, val);
}

int wsen_itds_slp_mode_sel(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_SLP_MODE_SEL, val);
}

int wsen_itds_h_lactive(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_H_LACTIVE, val);
}

int wsen_itds_lir(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_LIR, val);
}

int wsen_itds_pp_od(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_PP_OD, val);
}

int wsen_itds_st(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl3(driver, REG_CTRL3_ST, val);
}

int wsen_itds_update_ctrl4(i2c_driver_t driver, uint8_t reg, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, reg, val);
}

int wsen_itds_int0_drdy(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_DRDY, val);
}

int wsen_itds_int0_fth(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_FTH, val);
}

int wsen_itds_int0_diff5(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_DIFF5, val);
}

int wsen_itds_int0_tap(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_TAP, val);
}

int wsen_itds_int0_ff(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_FF, val);
}

int wsen_itds_int0_wu(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_WU, val);
}

int wsen_itds_int0_single_tap(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_SINGLE_TAP, val);
}

int wsen_itds_int0_6d(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl4(driver, REG_CTRL4_INT0_6D, val);
}

int wsen_itds_update_ctrl5(i2c_driver_t driver, uint8_t reg, uint8_t val) {
    return wsen_itds_update_ctrl2(driver, reg, val);
}

int wsen_itds_int1_drdy(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_DRDY, val);
}

int wsen_itds_int1_fth(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_FTH, val);
}

int wsen_itds_int1_diff5(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_DIFF5, val);
}

int wsen_itds_int1_ovr(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_OVR, val);
}

int wsen_itds_int1_drdy_t(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_DRDY_T, val);
}

int wsen_itds_int1_boot(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_BOOT, val);
}

int wsen_itds_int1_sleep_chg(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_SLEEP_CHG, val);
}

int wsen_itds_int1_sleep_state(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_ctrl5(driver, REG_CTRL5_INT1_SLEEP_STATE, val);
}

int wsen_itds_low_noise(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_register(driver, REG_CTRL6, 1 << REG_CTRL6_LOW_NOISE,
            val << REG_CTRL6_LOW_NOISE);
}

int wsen_itds_fds(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_register(driver, REG_CTRL6, 1 << REG_CTRL6_FDS,
            val << REG_CTRL6_FDS);
}

int wsen_itds_fs(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_register(driver, REG_CTRL6, REG_CTRL6_FS_MASK,
            val << REG_CTRL6_FS);
}

int wsen_itds_bw_filt(i2c_driver_t driver, uint8_t val) {
    return wsen_itds_update_register(driver, REG_CTRL6, REG_CTRL6_BW_FILT_MASK,
            val << REG_CTRL6_BW_FILT);
}

int wsen_itds_set_odr_2(i2c_driver_t driver) {
    return wsen_itds_bw_filt(driver, REG_CTRL6_BW_FILT_2);
}

int wsen_itds_set_odr_4(i2c_driver_t driver) {
    return wsen_itds_bw_filt(driver, REG_CTRL6_BW_FILT_4);
}

int wsen_itds_set_odr_10(i2c_driver_t driver) {
    return wsen_itds_bw_filt(driver, REG_CTRL6_BW_FILT_10);
}

int wsen_itds_set_odr_20(i2c_driver_t driver) {
    return wsen_itds_bw_filt(driver, REG_CTRL6_BW_FILT_20);
}

int wsen_itds_set_full_scale_2(i2c_driver_t driver) {
    return wsen_itds_fs(driver, REG_CTRL6_FS_2);
}

int wsen_itds_set_full_scale_4(i2c_driver_t driver) {
    return wsen_itds_fs(driver, REG_CTRL6_FS_4);
}

int wsen_itds_set_full_scale_8(i2c_driver_t driver) {
    return wsen_itds_fs(driver, REG_CTRL6_FS_8);
}

int wsen_itds_set_full_scale_16(i2c_driver_t driver) {
    return wsen_itds_fs(driver, REG_CTRL6_FS_16);
}

int wsen_itds_status_drdy(i2c_driver_t driver, uint8_t *val) {
    return i2c_read_register(driver, REG_STATUS, val, 1);
}

int wsen_itds_get_full_scale(i2c_driver_t driver, uint8_t *scale) {
    int res;
    uint8_t reg = 0;
    res = i2c_read_register(driver, REG_CTRL6, &reg, 1);
    if (res)
        return res;
    *scale = (reg & REG_CTRL6_FS_MASK) >> REG_CTRL6_FS;
    return res;
}

int wsen_itds_get_sensitivity(i2c_driver_t driver, float *sens) {
    int res;
    uint8_t mode, scale;
    res = i2c_read_register(driver, REG_CTRL1, &mode, 1);
    if (res)
        return res;
    mode = (mode & REG_CTRL1_MODE_MASK) >> REG_CTRL1_MODE_SHIFT;
    res = i2c_read_register(driver, REG_CTRL1, &mode, 1);
    if (res)
        return res;
    res = wsen_itds_get_full_scale(driver, &scale);
    if (res)
        return res;
    *sens = 0;
    if (mode == REG_CTRL1_MODE_NORMAL || mode == REG_CTRL1_MODE_HIGH_PERF) {
        switch (scale) {
            case REG_CTRL6_FS_2:
                *sens = 0.244;
                break;
            case REG_CTRL6_FS_4:
                *sens = 0.488;
                break;
            case REG_CTRL6_FS_8:
                *sens = 0.976;
                break;
            case REG_CTRL6_FS_16:
                *sens = 1.952;
                break;
        }
    } else {
        switch (scale) {
            case REG_CTRL6_FS_2:
                *sens = 0.976;
                break;
            case REG_CTRL6_FS_4:
                *sens = 1.952;
                break;
            case REG_CTRL6_FS_8:
                *sens = 3.904;
                break;
            case REG_CTRL6_FS_16:
                *sens = 7.808;
                break;
        }
    }

    return res;
}

int wsen_itds_init(i2c_driver_t driver) {
    int res;
    res |= wsen_itds_set_ord(driver, REG_CTRL1_ORD_12_5HZ_12_5HZ_1_6HZ);
    res |= wsen_itds_set_normal_mode(driver);
    res |= wsen_itds_set_full_scale_2(driver);
    uint8_t old_val;
    res |= i2c_read_register(driver, REG_CTRL1, &old_val, 1);
    return res;
}

int wsen_itds_get_coord(i2c_driver_t driver, uint8_t reg_l, uint8_t reg_h,
        int16_t *coord) {
    int res;

    uint8_t high = 0, low = 0;
    uint8_t mode;
    float sens;

    //  res = wsen_itds_get_sensitivity(driver, &sens);
    //  if (res)
    //    return res;

    res = i2c_read_register(driver, REG_CTRL1, &mode, 1);
    if (res)
        return res;
    mode = (mode & REG_CTRL1_LP_MODE_MASK) >> REG_CTRL1_LP_MODE_SHIFT;


    res = i2c_read_register(driver, reg_l, &low, 1);
    if (res)
        return res;

    res = i2c_read_register(driver, reg_h, &high, 1);
    if (res)
        return res;


    // 14 bits
    int16_t c = ((uint16_t) (high << 8) | (uint16_t) (low)) >> 2;
    int16_t m = 0x2000;
    int16_t mask = 0x3FFF;

    if (mode == REG_CTRL1_LP_MODE_LOW) {
        // 12 bits in low power mode
        c = c >> 2;
        m = 0x800;
        mask = mask >> 2;

        Nop();
        Nop();
        Nop();
    }

    // is negative
    if (c & m) {
        // invert two's complement
        c = (~c + 1) & mask;
    }

    *coord = c;

    return res;
}

int wsen_itds_get_all_coords2(i2c_driver_t driver, int16_t coord[3]) {
    uint8_t ready, mode;
    int res;

    wsen_itds_status_drdy(driver, &ready);
    // no data
    if (ready == 0)
        return 1;

    uint8_t start = REG_X_OUT_L;
    int8_t data[6] = {0,0,0,0,0,0};

    res = driver.i2c_transfer(driver.device_address, &start, 6, data, 6, NULL);
    if (res)
        return res;
    Nop();
    Nop();
    Nop();

    res = i2c_read_register(driver, REG_CTRL1, &mode, 1);
    if (res)
        return res;
    mode = (mode & REG_CTRL1_LP_MODE_MASK) >> REG_CTRL1_LP_MODE_SHIFT;

    int16_t m = 0x2000;
    int16_t mask = 0x3FFF;

    if (mode == REG_CTRL1_LP_MODE_LOW) {
        // 12 bits in low power mode
        m = 0x800;
        mask = mask >> 2;
    }
    
    Nop();
    Nop();
    Nop();

    int i;
    int j;
    for (i = 0, j = 0; i < 6; i += 2) {
        coord[j] = ((uint16_t)data[i] << 8 | (uint16_t)data[i + 1]) >> 2;
        if (mode == REG_CTRL1_LP_MODE_LOW)
            coord[j] = coord[j] >> 2;

        // is negative
        if (coord[j] & m) {
            // invert two's complement
            coord[j] = (~coord[j] + 1) & mask;
        }
        j++;
    }
    
    Nop();
    Nop();
    Nop();

    return res;
}

int wsen_itds_get_all_coords(i2c_driver_t driver, int16_t *coord) {
    int res, ready;
    wsen_itds_status_drdy(driver, &ready);
    // no data
    if (ready == 0) {
        Nop();
        Nop();
        Nop();
        return 1;
    }

    int16_t coord_x = 0, coord_y = 0, coord_z = 0;
    res = wsen_itds_get_x(driver, &coord_x);
    if (res)
        return res;
    res = wsen_itds_get_y(driver, &coord_y);
    if (res)
        return res;
    res = wsen_itds_get_z(driver, &coord_z);
    coord[0] = coord_x;
    coord[1] = coord_y;
    coord[2] = coord_z;

    return res;
}

int wsen_itds_get_x(i2c_driver_t driver, int16_t *coord) {
    return wsen_itds_get_coord(driver, REG_X_OUT_L, REG_X_OUT_H, coord);
}

int wsen_itds_get_y(i2c_driver_t driver, int16_t *coord) {
    return wsen_itds_get_coord(driver, REG_Y_OUT_L, REG_Y_OUT_H, coord);
}

int wsen_itds_get_z(i2c_driver_t driver, int16_t *coord) {
    return wsen_itds_get_coord(driver, REG_Z_OUT_L, REG_Z_OUT_H, coord);
}
