# Manual de usuario:

Entrar a la carpeta Toolchain y ejecutar make all dentro de la misma.
En la carpeta principal, ejecutar make all.
Correr el programa utilizando ./run.sh o sudo ./run.sh si qemu requiere permisos adicionales.

Se abrirá una ventana con el sistema operativo ejecutando el módulo de código del usuario, en nuestro caso, la shell. Desde aquí tendremos el poder de ejecutar los siguientes comandos: inforeg, ticks, printArgs, clock, printmem, triggerException0, triggerException6, beep y algunas canciones.

Además, la shell cuenta con el comando ‘help’ que se puede utilizar sin argumentos, y de esta manera la función muestra todos los comandos disponibles con una breve descripción, y la manera en como usarlo. También se puede pedir ayuda sobre una función en particular, ejecutando ‘help <COMMAND>’.

## Kernel:
### Drivers
En primer instancia, se tuvieron que programar los drivers necesarios para poder trabajar durante el tp. Entre estos driver se encuentran el de video, pantalla, sonido y teclado. 
Dado que el trabajo debía realizarse en **modo video**, lo primero que se programó fue el driver de video, en el que se obtienen valores tales como la cantidad de pixeles horizontales y verticales, que se deben cargar llamando a la función init_VM_driver al iniciar el sistema operativo. A su vez, aquí se encuentran las funciones provistas por la cátedra para dibujar un pixel dándole la posición del mismo y su color, y la función para dibujar un  caracter, dependiendo de la función charBitmap del archivo font.c, también provisto por la cátedra, que se ocupa de devolver el mapa de pixeles que deberá seguir la funcion para dibujar el carácter deseado. Se agrego la funcion copiar pixel que permite dado una posición fuente y una posición destino dibujar sobre la segunda el pixel que se encontraba en la primera.
Una vez terminado el driver de video se procedió a programar el driver de pantalla. Este brinda la funcionalidad de poder considerar a la pantalla como una matriz de caracteres, similar al modo texto. Para que este funcione correctamente se deberá ejecutar la función init_screen, que se ocupa de delimitar los valores de la matriz en base a la altura y ancho de los caracteres usados y la cantidad de pixeles horizontales y verticales, provistos por el driver de video. Se proveen funciones tales como mover el “cursor”, osea, la posición actual en la matriz, escribir un carácter, ya sea con formato o con los valores default, o del mismo modo imprimir un string. Se tomaron las consideraciones necesarias para el caso de ciertos caracteres especiales tales como el enter o el backspace. A su vez se añade la función de limpiar la pantalla y de subir todos las posiciones una línea, borrando la primera y dejando vacía la última. Consideramos que esta última corresponde en kernel, ya que este modo está pensado para ser usado usualmente por texto y siempre será deseable bajar una línea cuando se alcanza el final de la pantalla.
Para el driver de teclado, se tuvieron que tener ciertas consideraciones. En primer instancia se tuvo que habilitar el PIC para que reciba interrupciones del teclado. Se consideró un teclado en inglés, en el que solo se aceptaban las teclas que posean ASCII dejando afuera a ciertos caracteres como los cursores. Se realizaron las funciones para recibir el codigo del caracter correspondiente usando una tabla de conversión para obtener el ASCII deseado. Los caracteres obtenidos van guardando en un buffer, dándole al usuario la función correspondiente para obtener un carácter del mismo. Al leer las letras se considera si están o no habilitada la tecla de BLOQ MAYUS o si la tecla SHIFT fue presionada y no se ha recibido el breakcode que indique que fue liberada. No usamos los registros correspondientes a estas consideraciones, sino que se fueron analizando los scancode recibidos.
En el caso del driver del reloj, se provee una función que recibe un parámetro, y a partir de este decide devolver la hora, los minutos o los segundos actuales en formato 24 hs.
Por último, el driver de sonido, se ocupó de brindar la funcionalidad  de poder hacer sonar al PC Speaker enviándole una frecuencia y un tiempo de duración haciendo que este  emita el sonido correspondiente a la frecuencia recibida. esperando el tiempo dado y luego silenciando el sonido. Para esto se utilizó codigo de fuentes externas.
### Interrupciones
Para trabajar con las interrupciones se utilizó el código provisto por la cátedra, en el que, al recibirse una interrupción, se ocupa de ejecutar la rutina de atención correspondiente (habiendo previamente cargado sus descriptores en la idt) llamando a la función que se ocupará de realizar la acción deseada. Por ejemplo en el caso de recibir una interrupción de teclado, guardar el scancode obtenido. 
Del mismo modo, se programó la función encargada de atender las syscalls, la cual en base al parámetro recibido en el registro RDI, ejecutará la rutina de atención correspondiente, asumiendo que se pasaran por los demás registros los parámetros necesarios para la misma, en el orden que corresponda. Para que dichas llamadas a sistema funcionen se cargó la interrupción 80h de la idt. 
Las syscalls que decidimos ofrecer al usuario desde kernel fueron:
- TimerTick: Devuelve la cantidad de tick desde que se inició el sistema. P ara esto fue necesario desenmascarar la IRQ 0.
- DrawPixel: Colorea un píxel específico de la pantalla.
- sysWrite: Imprime a pantalla donde esté el cursor un string con el color de letra y fondo provistos.
- getChar: Devuelve la tecla tecla más vieja que keyboardDriver tenga almacenada en el buffer. Devuelve 0 si el buffer está vacío.
- setCursorPos: Posiciona el cursor en la posición especificada de la pantalla.
- VerticalPixelCount y HorizontalPixelCount: devuelve la cantidad de pixeles que tiene de alto y ancho la pantalla.
- getTime: Devuelve la hora, minutos o segundos actuales.
- sysBeep: Ejecuta un sonido por el PC Speaker en base a la frecuencia y - - tiempo provistos.
### Excepciones
Por último, para las excepciones se siguió el mismo procedimiento. Se tomaron en cuenta la excepción de división por cero y la excepción código de operación inválido. Las mismas se cargaron en la posición 0 y 6 de la idt respectivamente. Al ocurrir una interrupción, se procederá a imprimir en pantalla un mensaje que indique la excepción ocurrida e imprimiendo el estado actual de todos los registros. Se deja un tiempo para que el usuario pueda consultar la información dada y posteriormente se procede a volver al estado inicial del programa principal. Para poder lograr esto, antes de inicializar SampleCodeModule, se guardan las direcciones del IP y del stack, justo antes de ejecutar el programa principal, y antes de realizar el iretq de excepción se modifican los valores del stack y del instruction pointer del stackframe recibido, por dichos valores de back up, asegurando que el programa volverá a comenzar correctamente y no se sobrecarga el stack con las nuevas ejecuciones del programa.

## Userland:
En esta zona del código se cuenta con una librería de usuario donde se brindan funcionalidades de uso general. Se brinda funcionalidad para strings cómo imprimir (con o sin formato), comparar strings, obtener el largo, poder parsear strings y poder convertir un string correspondiente a un número hexadecimal a su valor entero. A su vez se cuenta con funciones tales como la potencia, obtener los valores de alto y ancho de la “matriz” de caracteres considerada en el driver de pantalla y borrar la pantalla, entre otras. Algunas de estas funciones poseen toda su lógica en C y otras utilizan funciones de assembler para poder ejecutar las llamadas a sistema que necesiten.
El trabajo se modelo teniendo como programa central una shell, desde la cual se podrán ejecutar las distintas funcionalidades realizadas llamandolas por línea de comando. En todo momento se puede presionar la tecla ESC para salir de la terminal.
Para poder ejecutar las funciones por comando se decidió realizar un arreglo de “Paquetes de funciones”, que cuentan con el nombre, la descripción de la función y el puntero a la misma. Se utiliza loadFunctions como método para cargar los valores de este array, en donde se hace un loadFunction por cada una de las funciones cargando los valores a la estructura de la posición correspondiente. 
Al comenzar la shell, se carga el array de funciones, se limpia la pantalla y se entra a un ciclo donde constantemente se estará preguntando por la entrada del usuario, hasta que el mismo escriba un enter o se llegue a la máxima longitud permitida para un comando. Luego se llamará a la función processInstruction con el input recibido por el usuario. Esta procederá a parsear el input y se fijará si el primer argumento coincide con el nombre de alguna de las funciones cargadas, ejecutándola de ser el caso, o enviando un mensaje de error en caso contrario. Finalmente la función dejara el cursor en la última línea, permitiendo al usuario continuar escribiendo.
### Modulos disponibles
Se cuenta con la función help que dará información acerca de todas las funcionalidades ejecutables dentro de la terminal. A su vez se la puede invocar con el nombre de alguna función existente como parámetro para recibir sólo la descripción de esta.
La función inforeg, se ocupará de imprimir a pantalla el estado de todos los registros al invocarse esta función teniendo en cuenta el RIP que indica la dirección a la que está apuntando el instruction pointer y el RSP que indica la dirección a la que apunta el puntero de stack. Debido a la implementación utilizada, los valores que se imprimen de rdi y rsi son los que se utilizan para el pasaje de los parámetros al llamar la función, no se puede mantener su valor original. De todas formas, consideramos que no es algo contraproducente ya que, estos registros no reflejan el estado de la shell. Si se quisiese utilizar este programa en un entorno de debugeo, se le podría remover los parámetros que recibe y llamar directamente.
La función printArgs, imprime a pantalla los argumentos con los que fue invocada, dejando un enter entre cada uno. 
La función clock se encarga de imprimir la hora actual, teniendo en cuenta que el procesador envía las horas con un adelanto de 3 en relación a nuestro sistema horario. La hora es impresa en formato HH:MM:SS. Puede invocarse la función con los parámetros -s, -m o -h para obtener únicamente los segundos minutos u horas, respectivamente.
La  función printMem recibe una dirección de memoria como argumento  e imprime las direcciones de las siguientes 32 direcciones de memoria, con su valor en hexa abreviado.
Las funciones triggerException0 y triggerException6 se encargan de simular casos donde se vayan a desencadenar las correspondientes excepciones para poder comprobar el correcto procedimiento de las mismas.
La función beep se encarga de realizar una llamada a sistema a la syscall 8 tocando un LA por 5 segundos.
Las funciones Lavander, Elisa, Evangelion, SadMusic, Victory y Defeat hacen sonar música usando el PC Speaker teniendo en cuenta que a determinadas frecuencias se  pueden conseguir las notas de una escala musical, definiendo cada una en un enum en el archivo music.c. 
La función Arkanoid se encarga de iniciar el juego. Se puede iniciar una nueva partida o se puede continuar una partida ya iniciada. El objetivo del juego consiste en remover todos los bloques haciendo rebotar la pelota con la barra inferior. Si la pelota pasa el nivel de la barra 3 veces la partida se da por finalizada.
Para la realización del juego se decidió que la cantidad de bloque involucrados sea en función del tamaño de la pantalla y ocupen todo el ancho de la misma. Para esto se necesita que la pantalla cumpla con dimensiones mínimas definidas, de no cumplir los requisitos, el juego no se ejecutará. 
Se modeló la pelota como una entidad con 4 parámetros característicos. Los primeros dos son su ubicación en la pantalla (xc, yc) y los otros dos son su velocidad (vx, vy), es decir en cuantos pixel y en qué sentido se moverá el centro en cada tick. 
Para el movimiento de la barra se asignaron las letras A y D, las cuales le permiten desplazarse hacia la izquierda y la derecha respectivamente. Para el movimiento de la pelota se utilizó el concepto de velocidad ya descrito y se realizó una lógica de colisiones donde, se revisa si la pelota chocó con los límites de la pantalla o si choco contra un bloque ( en este caso se realizan las acciones de borrado del ladrillo). 
La cantidad de vidas restantes, los ladrillos que faltan romper para ganar y cuantos ticks ocurrieron desde el inicio de ejecución del juego son mostrados en la parte superior de la pantalla. A su vez, se incrementa la velocidad del juego (ejecutando la función de movimiento de ball mas de una vez por tick) en intervalos de 15 segundos hasta alcanzar una velocidad máxima (4 ejecuciones por tick). Cuando se pierde una vida, la pelota arranca con la velocidad mínima y acelera hasta llegar a la velocidad máxima.
El juego permite volver a la shell en cualquier momento utilizando la tecla de Tabulación. Luego, desde shell, se puede utilizar la misma tecla para volver directamente al juego y continuar desde el estado que había sido dejado. La opción de volver a shell solo se puede usar si hay una partida en transcurso (se inició una partida y se la abandonó con tab), sino la tecla tabulación actuará de manera normal. Llamar a la función arkanoid con el argumento -c tiene el mismo efecto que apretar tab desde shell.
Una vez concluida la partida, ya sea que el jugador perdió o que gano, se le da la opción de volver a la consola o de volver a jugar. Para salir del juego hay dos opciones, utilizando la tecla Escape para terminar la partida y Tabulación para dejar la partida en pausa, la cual podrá ser continuada.
Por último, las variables que llevan registro de los bloques presentes, la posición de la pelota, la barra y las vidas, es decir, las variables involucradas con el estado de la partida, son globales. El objetivo de esto fue poder implementar un guardado facil de partida en donde no hay que manejar variables externas al juego, de forma que, él mismo se encarga de tener toda la información necesaria para reanudar la partida. Esto tiene un aspecto negativo, el espacio requerido por este estado interno (el cual no es muy grande) siempre estará reservado, se quiera jugar a Arkanoid o no.