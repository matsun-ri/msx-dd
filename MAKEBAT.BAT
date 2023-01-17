echo . 
echo --- dd_scrn --- 
cf -j -r99:55:30 dd_scrn 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_scrn 
m80 =dd_scrn/z 
del dd_scrn.mac 
echo . 
echo --- dd_msg --- 
cf -j -r99:50:30 dd_msg 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_msg 
m80 =dd_msg/z 
del dd_msg.mac 
echo . 
echo --- dd_input --- 
cf -j -r99:65:30 dd_input 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_input 
m80 =dd_input/z 
del dd_input.mac 
echo . 
echo --- dd_sub --- 
cf -j -r6:4:2 dd_sub 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_sub 
m80 =dd_sub/z 
del dd_sub.mac 
echo . 
echo --- dd_drv --- 
cf -j -r63:41:20 dd_drv 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_drv 
m80 =dd_drv/z 
del dd_drv.mac 
echo . 
echo --- dd_file --- 
cf -j -r80:45:25 dd_file 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_file 
m80 =dd_file/z 
del dd_file.mac 
echo . 
echo --- dd_file2 --- 
cf -j dd_file2 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_file2 
m80 =dd_file2/z 
del dd_file2.mac 
echo . 
echo --- dd_exec --- 
cf -j -r6:4:2 dd_exec 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_exec 
m80 =dd_exec/z 
del dd_exec.mac 
echo . 
echo --- dd_init --- 
cf -j -r6:4:2 dd_init 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_init 
m80 =dd_init/z 
del dd_init.mac 
echo . 
echo --- dd_copy --- 
cf -j -r6:4:2 dd_copy 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_copy 
m80 =dd_copy/z 
del dd_copy.mac 
echo . 
echo --- dd_opr2 --- 
cf -j -r62:40:20 dd_opr2 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd_opr2 
m80 =dd_opr2/z 
del dd_opr2.mac 
echo . 
echo --- dd_opr --- 
cf -j -r63:42:20 dd_opr 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg  dd_opr 
m80 =dd_opr/z 
del dd_opr.mac 
echo . 
echo --- dd_key --- 
cf -j -r5:5:2 dd_key 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg  dd_key 
m80 =dd_key/z 
del dd_key.mac 
echo . 
echo --- dd --- 
cf -j -r32:22:11 dd 
fpc -s lib mlib dd_scrn dd_msg dd_input dd_sub dd_drv dd_file dd_file2 dd_exec dd_init dd_copy dd_opr dd_opr2 dd_key dd 
cg dd 
m80 =dd/z 
del dd.mac 
copy dd_drv.rel  dr.* 
copy dd_exec.rel dx.* 
copy dd_file.rel df.* 
copy dd_file2.rel dg.* 
copy dd_msg.rel  dm.* 
copy dd_input.rel di.* 
copy dd_scrn.rel ds.* 
copy dd_sub.rel  db.* 
copy dd_init.rel dn.* 
copy dd_copy.rel dc.* 
copy dd_opr2.rel dp.* 
copy dd_opr.rel  do.* 
copy dd_key.rel  dk.* 
l80 ck,ds,dm,di,db,dr,df,dg,dx,dn,dc,dp,do,dk,dd,mlib/s,clib/s,crun/s,cend, dd/n/e 
rem l80 ck,dd_scrn,dd_msg,dd_input,dd_sub,dd_drv,dd_file,dd_exec,dd_init,dd_copy,dd_opr2,dd_opr,dd_key,dd,mlib/s,clib/s,crun/s,cend, dd/n/e 
