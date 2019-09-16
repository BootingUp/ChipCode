# TVP5150_driver
TVP5150
设备树配置

###
~~~

&vip2 {  
    status = "okay";  
};  

&vin3a {  
    status = "okay";  

    endpoint {  
	    slave-mode;  
	    remote-endpoint = <&tvp5150_5c>;  
    };  
};  
  
&i2c1 {  
	status = "okay";
	clock-frequency = <400000>;

	tvp5150@5c {
		     compatible = "ti,tvp5150";
		     reg = <0x5c>;
		     pdn-gpios = <0>;  
		     reset-gpios = <&gpio3 12 GPIO_ACTIVE_LOW>;  

		     port {
			      tvp5150_5c: endpoint {  
				    remote-endpoint = <&vin3a>;  
				    hsync-active = <1>;  
				    vsync-active = <1>;  
				    bus-width = <8>;  
           			    channels = <0>;  
			     };  
		     };  

	};  
   };  
 
~~~
###
