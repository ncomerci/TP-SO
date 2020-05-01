// -------------- Readme.txt -------------- //

Con respecto a la entrega parcial, entregamos:

- Los 2 Memory Managers (Buddy System & First Fit - heap4 from freeRTOS - ) bajo una misma interfaz.
- Scheduler con Priority Based Round Robin (Se intentó agregar Aging aunque se encuentra temporalmente deshabilitado, además la condición es muy pequeña, para probarlo, utilizar una cantidad de quantums más apropiada).
- Aplicaciones solicitadas:
    - Respecto del Memory Manager:
        - mem
        - test mem (agregada para probar malloc y free)
    - Respecto de Procesos:
        - ps
        - loop
        - kill
        - nice
        - block
        - test process (se hizo para probar que si la shell creaba un proceso en primer plano, este ocupaba el foreground y al retornar, volvía a READY la shell.)

Respecto de estas aplicaciones, su respectivo uso se puede encontrar utilizando el comando <help>.

Problemas encontrados sin resolver:
    - Si bien no es requisito para esta entrega, se intentó portear a ultimo momento el test de memoria brindado, y se obtuvieron problemas a la hora de querer insertar un caracter mientras esta aplicación corre (para ver el problema, se puede llamar a <test mm>).
    - Se intentó utilizar espera bloqueante para el caso de loop, lo cual terminó funcionando. Sin embargo, esto nos trajo un problema que descubrimos a ultima hora y que no pudimos resolver ya que no se solicitaba para esta entrega:
        El proceso loop realmente es llamado cada 1 segundo, manteniendose bloqueado mientras tanto. Sin embargo, al llamarse <block> desde la Shell (intentando bloquearlo) se cambia el estado del proceso pues esta es la verdadera funcionalidad del comando <block>, lo cual en realidad desbloquea el proceso que se encontraba bloqueado ante la espera del timer. Descubrimos un comportamiento inesperado, donde al ocurrir esto, el proceso loop parece agregarse de más en la cola, con lo cual corre más timer ticks consecutivos que antes.
    - Además, se intentó agregar el atajo de CTRL + C para terminar con la ejecución del proceso foregrounder (a menos que sea la Shell), lo cual por el momento no está funcionando, tal como se puede ver si se aplica CTRL + C durante la ejecución de <test process>, si bien este tampoco es un requisito para esta entrega.