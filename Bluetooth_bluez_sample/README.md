* **bluetooth系统构成**  

**蓝牙主机**    
应用程序(Application)  
高层协议(Higher Layer)  
主机控制接口(HCI)  
     |   
     |  
     |  
 主机控制器（host controller)  
  链路管理(LinkManager)                   蓝牙音频(audio)  
  
        基带与链路控制(baseband & linkController)  
        
        射频(Radio)  
        
        蓝牙设备
        
```  
1、无线射频单元(Radio)：负责数据和语音的发送和接收，特点是短距离、低功耗。蓝牙天线一般体积小、重量轻，属于微带天线。 
2、基带或链路控制单元(LinkController)：进行射频信号与数字或语音信号的相互转化，实现基带协议和其它的底层连接规程。 
3、链路管理单元(LinkManager)：负责管理蓝牙设备之间的通信，实现链路的建立、验证、链路配置等操作。 
```  

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

```
$ sudo rfkill list
0: hci0: Bluetooth
Soft blocked: no
Hard blocked: no

```

如果设备被 `blocked`, 意味着不可用, 需要执行 `unblock` 操作, 如 `sudo rfkill unblock <id>`.


# **1. HCI层协议概述**

HCI提供一套统一的方法来访问Bluetooth底层, 用来沟通Host和Module , Host通常就是PC , Module则是以各种物理连接形式（USB,serial,pc-card）连接到PC上的bluetooth Dongle。 

**Hos端**：application,SDP,L2cap等协议都是软件形式提出的（Bluez中是以kernel层程序)。

**Module端**：Link Manager, BB, 等协议都是硬件中firmware提供的。 

 HCI  它一部分在软件中实现，用来给上层协议和程序提供访问接口（Bluez中hci.c hci_usb.c hci_sock.c等）.另一部分也是在Firmware中实现，用来将软件部分的指令方式传递给底层。 



HCI有4种不同形式的传输：**Commands, Event, ACL Data, SCO/eSCO Data**

1、 HCI Command是Host向Modules发送命令的一种方式。 

2、 HCI Event是 Modules向Host发送一些信息 

3、ACL、SCO数据是 l2cap数据是通过ACL数据传输给remote device。 一个l2cap包会按照规则先切割为多个HCI数据包。HCI数据包再通过HCI-usb这一层传递给USB设备。每个包又通过USB driver发送到底层。 

# **1. HCI层协议概述**

HCI提供一套统一的方法来访问Bluetooth底层, 用来沟通Host和Module , Host通常就是PC , Module则是以各种物理连接形式（USB,serial,pc-card）连接到PC上的bluetooth Dongle。 

**Hos端**：application,SDP,L2cap等协议都是软件形式提出的（Bluez中是以kernel层程序)。

**Module端**：Link Manager, BB, 等协议都是硬件中firmware提供的。 

 HCI  它一部分在软件中实现，用来给上层协议和程序提供访问接口（Bluez中hci.c hci_usb.c hci_sock.c等）.另一部分也是在Firmware中实现，用来将软件部分的指令方式传递给底层。 



HCI有4种不同形式的传输：**Commands, Event, ACL Data, SCO/eSCO Data**

1、 HCI Command是Host向Modules发送命令的一种方式。 

2、 HCI Event是 Modules向Host发送一些信息 

3、ACL、SCO数据是 l2cap数据是通过ACL数据传输给remote device。 一个l2cap包会按照规则先切割为多个HCI数据包。HCI数据包再通过HCI-usb这一层传递给USB设备。每个包又通过USB driver发送到底层。 



##  **HCI 层的编程** 

 **得到Host上插入Dongle数目以及Dongle信息** 

 使用socket() 打开一个HCI protocol的socket，表明得到这个房间的句柄。HOST可能会有多个Dongle。换句话说，这个房间可以有多个电话号码。所以HCI会提供一套指令去得到这些Dongle。 

**1、分配一个空间给 hci_dev_list_req。存放所有Dongle信息**

```c
struct hci_dev_list_req *dl;
struct hci_dev_req *dr;
struct hci_dev_info di;

int i;
if (!(dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t)))) {
  perror("Can't allocate memory");
  exit(1);
}
dl->dev_num = HCI_MAX_DEV;
dr = dl->dev_req;	
```

​	**2、打开一个HCI socket**	

```c
if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) {
  perror("Can't open HCI socket.");
  exit(1);
 }
```

**3、 使用HCIGETDEVINFO，得到对应Device ID的Dongle信息** 

```c
struct hci_dev_info di;
 /* 得到所有Dongle信息 */
if (ioctl(ctl, HCIGETDEVLIST, (void *) dl) < 0) {
        perror("Can't get device list");
        free(dl);
        exit(1);
}   

 for (i = 0; i< dl->dev_num; i++) 
 {
        di.dev_id = (dr+i)->dev_id;

        /* get hci device info*/
        if (ioctl(ctl, HCIGETDEVINFO, (void *) &di) < 0)
            continue;

        printf("%d %s \n", di.dev_id, di.name);
        int j = 0;
        for (j = 0; j < sizeof(di.bdaddr.b); j++)
        {
            printf("%02x ", di.bdaddr.b[j]);
        }
        printf("\n");
}   

struct hci_dev_info {
 uint16_t dev_id;   //dongle Device ID
 char     name[8];  //Dongle name

 bdaddr_t bdaddr;   //Dongle bdaddr

 uint32_t flags;    //Dongle Flags：如：UP，RUNING，Down等。
 uint8_t  type;   //Dongle连接方式：如USB，PC Card，UART，RS232等。

 uint8_t  features[8];

 uint32_t pkt_type;
 uint32_t link_policy;
 uint32_t link_mode;

 uint16_t acl_mtu;
 uint16_t acl_pkts;
 uint16_t sco_mtu;
 uint16_t sco_pkts;

 struct   hci_dev_stats stat;  //此Dongle的数据信息
};
```

 **3、UP和Down Bluetooth Dongle** 

```c
  	int hdev = 0; //hci0
	/* Start HCI device */
    if (ioctl(ctl, HCIDEVUP, hdev) < 0) {
        if (errno == EALREADY)
            return;
        fprintf(stderr, "Can't init device hci%d: %s (%d)\n",
                hdev, strerror(errno), errno);
        exit(1);
    }

	 /* Stop HCI device */
    if (ioctl(ctl, HCIDEVDOWN, hdev) < 0) {
        fprintf(stderr, "Can't down device hci%d: %s (%d)\n",
                hdev, strerror(errno), errno);
        exit(1);
    }

```

#  L2CAP

l2CAP是Linux Bluetooth编程的基础。几乎所有协议的连接，断连，读写都是用L2CAP连接来做的。

 

**1.创建L2CAP Socket：**

```c
socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP);  

domain=PF_BLUETOOTH,  

protocol=BTPROTO_L2CAP.  
```

**2.绑定：**

```c
// Bind to local address
 memset(&addr, 0, sizeof(addr));
 addr.l2_family = AF_BLUETOOTH;
 bacpy(&addr.l2_bdaddr, &bdaddr);  //bdaddr为本地Dongle BDAddr

 if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
 perror("Can't bind socket");
 goto error;
 }
```

 

**3.连接**

```c++
memset(&addr, 0, sizeof(addr));
addr.l2_family = AF_BLUETOOTH;
bacpy(addr.l2_bdaddr, src);

addr.l2_psm = xxx; 

if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
 	perror("Can't connect");
 	goto error;
}
```

**注意：**

**struct sockaddr_l2 {**  
 **sa_family_t l2_family;  //必须为 AF_BLUETOOTH**  
 **unsigned short l2_psm;  //与前面PSM对应,这一项很重要**  
 **bdaddr_t l2_bdaddr;   //Remote Device BDADDR**  
 **unsigned short l2_cid;**   
**};**  



**4. 发送数据到Remote Device：**

send()或write()都可以。

**5. 接收数据：**

revc() 或read()



# **BlueZ提供的HCI编程接口** 

 **打开一个HCI Socket** 

int hci_open_dev(int dev_id);

**关闭一个HCI Socket**

int hci_close_dev(int dd);

**将BDADDR转换为字符串**

int ba2str(const bdaddr_t *ba, char *str)

**将自串转换为BDADDR**

int str2ba(const char *str, bdaddr_t *ba)

**inquiry 远程Bluetooth Device**

int hci_inquiry(int dev_id, int len, int nrsp, const uint8_t *lap, inquiry_info **ii, long flags)  

hci_inquiry（）用来命令指定的Dongle去搜索周围所有bluetooth device.并将搜索到的Bluetooth   Device bdaddr 传递回来。

参数1：dev_id：指定Dongle Device ID。如果此值小于0，则会使用第一个可用的Dongle。

参数2：len: 此次inquiry的时间长度（每增加1，则增加1.25秒时间）

参数3：nrsp:此次搜索最大搜索数量，如果给0。则此值会取255。

参数4：lap:BDADDR中LAP部分，Inquiry时这块值缺省为0X9E8B33.通常使用NULL。则自动设置。

参数5：ii:存放搜索到Bluetooth Device的地方。给一个存放inquiry_info指针的地址，它会自动分配空间。并把那个空间头地址放到其中。

参数6：flags:搜索flags.使用IREQ_CACHE_FLUSH，则会真正重新inquiry。否则可能会传回上次的结果。

返回值是这次Inquiry到的Bluetooth Device 数目。

 

**获取指定BDAddr的reomte device Name**

int hci_read_remote_name(int dd, const bdaddr_t *bdaddr, int len, char *name, int to)

参数1：使用hci_open_dev（）打开的Socket。

参数2：对方BDAddr.

参数3：name 长度。

参数4：(out)放置name的位置。

参数5：等待时间。

 

**读取连接的信号强度**

int hci_read_rssi(int dd, uint16_t handle, int8_t *rssi, int to)

注意，所有对连接的操作，都会有一个参数，handle.这个参数是连接的Handle。前面讲过如何得到连接Handle的。
