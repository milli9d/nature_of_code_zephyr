// static void tft_test()
// {
//     static noc::frame_buffer frame_buf(TFT_W, TFT_H, TFT_BPP);

//     // uint16_t color = rand() % UINT16_MAX;
//     // frame_buf.fill_buffer((uint8_t*)&color, sizeof(color));
//     frame_buf.fill_buffer_rand();

//     const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
//     if (!device_is_ready(dev)) {
//         printk("Device %s not ready\n", dev->name);
//         return;
//     }

//     if (display_set_pixel_format(dev, PIXEL_FORMAT_RGB_565) != 0) {
//         printk("Failed to set required pixel format\n");
//         return;
//     }

//     int rc = display_blanking_off(dev);
//     if (rc != 0) {
//         printk("Failed to turn on display blanking: %d\n", rc);
//     }

//     struct display_buffer_descriptor _desc = { .buf_size         = ((TFT_W * TFT_H) * TFT_BPP),
//                                                .width            = TFT_W,
//                                                .height           = TFT_H,
//                                                .pitch            = TFT_W,
//                                                .frame_incomplete = false };

//     rc = display_write(dev, TFT_X_OFF, TFT_Y_OFF, &_desc, (void*)frame_buf.get_buffer());
//     if (rc != 0) {
//         printk("Failed to write to display: %d\n", rc);
//     }
// }