<html><head>
<title>Show-Cluster CGI Script V2.2</title>
</head>
<body>
<h1>VMScluster Configuration:<br>List of Nodes</h1>
<h4>[Options: <a href="show-cluster.com?listnodes">Nodes</a> | <a href="show-cluster.com?listdisks">Disks</a> | <a href="show-cluster.com?selectprod">Products</a> | <a href="show-cluster.com?help">Help</a>]</h4>
<hr>
This cluster contains:
<ul>
<li>32 nodes (10 Alpha, 22 VAX)
<li>16 system disks
<li>8 versions of OpenVMS
</ul>
The operation system versions are:<ul><li>OpenVMS Alpha E7.1, V7.0, V6.2, V6.1
<li>OpenVMS VAX V7.0, V6.2, V6.1, V5.5</ul>
<hr><h2>List of Nodes</h2><pre>
Node    Address    Model                      Version
------  ---------  -------------------------  -------
AUBER   (47.894)   VAX 6000-430               V6.1
BBILON  (47.488)   VAXstation 3100            V6.2
CHAN05  (61.525)   VAXserver 3100             V5.5
CLPR01  (47.261)   VAX 6000-440               V7.0
CLPR08  (62.808)   MicroVAX 3600 Series       V6.1
CLPR11  (62.811)   VAXstation 3100/GPX        V5.5
CLUSKI  (47.411)   DEC 3000 Model 300L        V6.2
COKTEL  (62.105)   MicroVAX 3600 Series       V6.1
EPICDA  (47.635)   VAXstation 4000-90         V6.2
FALPHA  (47.6)     DEC 7000 Model 610         V7.0
GRAPH1  (61.543)   VAXserver 3400 Series      V6.1
LUTECE  (47.69)    DEC 3000 Model 400         V7.0
MALGRA  (47.509)   VAXstation 3100/GPX        V6.2
PADBOL  (47.180)   ALPH                       E7.1 <img src="/images/new.gif" align=bottom alt="[NEW]">
PADKOA  (47.211)   ALPH                       V6.2
PADNOM  (47.232)   ALPH                       V6.1
PADSOU  (47.444)   ALPH                       V7.0
PRSSOS  (47.91)    VAX 6000-430               V5.5
RESTO   (61.538)   VAXstation 4000-60         V6.2
RONE03  (61.103)   VAXstation 3100/GPX        V6.2
RONE06  (61.106)   VAXstation 3100/SPX        V5.5
RONE21  (61.121)   VAXstation 3100-M76/SPX    V6.1
RONE23  (61.123)   VAXstation 4000-60         V6.2
RONE28  (61.128)   VAXstation 3100/GPX        V6.1
RONE31  (61.131)   VAXstation 3100/SPX        V6.2
RONE35  (61.135)   VAXstation 3100/GPX        V5.5
SOS4    (47.212)   ALPH                       V7.0
SOS6    (47.214)   VAX 6000-420               V6.2
SOS7    (61.544)   DEC 3000 Model 300L        V6.2
SUPELN  (61.2)     VAXstation 3100/GPX        V6.1
SZAJBA  (47.196)   DEC 3000 Model 400         V6.2
TIMI    (47.491)   VAX 4000-200               V5.5
</pre>
</body></html>
