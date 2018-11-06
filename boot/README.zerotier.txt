Our version of the DTS for this board has changes to this section:

/* J1 */
&sdhci1 {
        wp-inverted;
        bus-width = <4>;
        cd-gpios = <&gpionb 3 GPIO_ACTIVE_LOW>;
        no-1-8-v;
        marvell,pad-type = "sd";
        post-power-on-delay-ms = <200>;
        vqmmc-supply = <&vcc_sd_reg1>;
        status = "okay";
};

We must increase the post power-on delay and set no-1-8-v for
compatibility with some micro SD cards, including the one shipped
with the Edge.

We also made one modification to the 4.19 kernel source. We replaced
the mvebu-uart.c driver with the one from 4.14, which seems to work
quite a bit better. Might be able to drop this once Linux upstream
fixes it. I beleive they are aware of the issues.
