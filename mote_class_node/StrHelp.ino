/*Sera mejor retornar el indice del string en que que quedo para luego iniciar el siguiente recorrido
  desde ahi y no desde el principio, son menos de 5 iteraciones pero puede ayudar algo?
*/
//Find something between ]ini, endi[
boolean start_proto(String &ms, String &searchi, char ini, char endi){
  // put your main code here, to run repeatedly:
  boolean is_in = false;
  uint8_t j = 0;
  for(uint8_t i = 0; i < searchi.length(); i++){
    if(ini == searchi.charAt(i)){
      is_in = true;
      j = i+1;
      while(endi != searchi.charAt(j)){
        ms += searchi[j];
        j++;
     }
     break;//Find just the first pair ini-endi
     i = j-1;
     return true;
    }  
  }//end for
  return false;
}//end start_proto
//int msg_spliter(){}//end msg_spliter
//int json_formater(){}//end json_formater

