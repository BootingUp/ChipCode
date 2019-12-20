
* **hciconfig**


查看及更改本机蓝牙设备的状态

```
$ sudo hciconfig  
hci0:	Type: BR/EDR  Bus: USB  
BD Address: F0:D5:BF:E7:0E:CC  ACL MTU: 8192:128  SCO MTU: 64:128  
UP RUNNING ISCAN   
RX bytes:1036 acl:0 sco:0 events:49 errors:0  
TX bytes:701 acl:0 sco:0 commands:49 errors:0  


```

可更改的状态: `UP`, `DOWN`, `PSCAN`(Page Scan状态表示设备可被连接), `ISCAN`(Inquiry Scan状态表示设备可被inquiry), `PISCAN`.


* **btmon**

监听蓝牙事件, 如蓝牙意外断开时, 就可以用它来看是收到什么事件断开的, 为进一步调试提供基础.


* **rfkill**

查看蓝牙及无线设备是否被 `blocked`, 如:

```shell
$ sudo rfkill list
0: hci0: Bluetooth
Soft blocked: no
Hard blocked: no

```

如果设备被 `blocked`, 意味着不可用, 需要执行 `unblock` 操作, 如 `sudo rfkill unblock <id>`.
