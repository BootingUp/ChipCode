# isl2026_driver
kernel 4.4.41  

**设备树配置**
  
 ~~~
&i2c4 {  
    status = "okay";  
    clock-frequency = <400000>;  
    x1226@6F {  
        compatible = "x1226";  
        reg = <0x6f>;  
    };    
};  
~~~
