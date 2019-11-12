# EFRBG13-bluetooth-mesh-embedded-provisioner
Embedded provisioner for EFR32 Blue Gecko BG13 


The packet format from BG13 to ESP32:
###example：
1LightsxxxxON_OFFxxxxxxxxxxxxxONxxx

* 0 : update or change value 
* 1-10: device type
* 11-30: attribute name
* 31-35: attribute value

