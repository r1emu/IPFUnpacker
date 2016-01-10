# IPFUnpacker
 - Usage : ipf_unpack.exe \<IPF file\> [decrypt/encrypt/extract]

#### "decrypt" feature
Decrypt feature will **replace** the encrypted IPF in argument by the decrypted one.  
Make sure to backup your IPF files somewhere before decrypting them.  
Once decrypted, the IPF is readable by traditional tools (such as IPF Suite).  

#### "encrypt" feature
Encrypt will restore a decrypted IPF in argument to an encrypted one.  
Make sure not encrypting twice the same IPF.

#### "extract" feature
Extract takes a decrypted IPF as argument and generates a list of files.
Some extension files aren't decrypted entirely ; In that case, only the MD5 of the decrypted is generated.

![HowTo](http://i.imgur.com/UJzXDZN.gif)
