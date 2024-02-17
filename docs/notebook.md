## 2024/02/17

Testing what acceleration level needs to be selected on the ADXL343 for rapidly waving the stick to not max it out.

- Range = 4G. Gravity = 128. MaxVal = 512
- Range = 8G. Gravity = 64. MaxVal = 511
- Range = 16G. Gravity = 32. MaxVal = 275

Because 8G maxxes out, it looks like we will need to use the 16G setting. However, it is not impossible that the 8G setting will turn out to be a better fit. When waving the stick at more reasonable speeds, the 16G max value was more like 200
