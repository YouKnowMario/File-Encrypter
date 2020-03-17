# File Encrypter
***
File and Directory Encrypter.  

## Help
***
usage: encrypter [options] [input file name] [password] [output file name]  
-h or --help	 prints help  
-e or --encrypt		encrypts the input file to the output file,  
if no output file is specified, default output file name will be used.   
-d or --decrypt		decrypts the input encrypted file or directory,  
 you don't need to specify output file name, the encrypted file name will be used.  
If output file is specified the output file/directory will be named as the output file name.  
-E			do what normal encryption option does and deletes the input file. 
-D			do what normal decryption option does and deletes the input encrypted file.  
-ed			do what normal encryption option does just with directory.  
-ED			do what normal directory encryption option does and deletes the input directory.  

## Encryption Algorithms 
***
* RC4  

in the future the encrypter will support more encryption algorithms.  

## Usage examples 
***
file encryption:  
```encrypter -e hello_world.png GoodPassword45 e_Hello_world.png```  
```encrypter -E hello_world.png GoodPassword45 e_Hello_world.png```  
folder encryption: note that the output is a file.     
```encrypter -ed folder GoodPassword45 e_folder```  
```encrypter -ED folder GoodPassword45 e_folder```  
file and folder decryption:  
```encrypter -d e_Hello_world.png GoodPassword45 hello_world.png```  
```encrypter -D e_Hello_world.png GoodPassword45 hello_world.png```  
```encrypter -d e_folder GoodPassword45 folder```  
```encrypter -D e_folder GoodPassword45 folder```  


