#####################################################
###	Creare file header, libreria jnichaos.so ed avvio Test:
#####################################################


1) Buildind del progetto Netbeans
2) Creare il file header da terminale dalla directory JNIChaos

	javah -jni -o JNIChaos.h -classpath build/classes it.infn.chaos.JNIChaos

4) Settare la variabile d'ambiente

	export JAVA_HOME=/usr/lib/jvm/java-8-oracle

5) Modificare il file CMakeLists.txt nella directory JNIChaos, aggiungendo: (SOLO LA PRIMA VOLTA)

	$ENV{JAVA_HOME}/include/linux		alla riga SET (set_local_include_path ...
	${PROJECT_SOURCE_DIR}/..		Come primo elemento alla riga INCLUDE_DIRECTORIES(. 

6) Compilare la parte in C

	mkdir cmakeBuild
	cd cmakeBuild
	cmake ..
	make

N.b:
	I primi 2 comandi possono essere evitati ma così evito di sporcare la directory.

7) Spostare i tre file .so nella directory superiore JNIChaos

8) Avviare progetto Test.java per il testing


########################################################
###	Conversione parametri formali in JNIChaos.cc
########################################################

    const char * parametroDaFornire = env->GetStringUTFChars(parametroRicevuto,0);

# puntatore ad un intero uint32_t:

    jclass clazz = env->GetObjectClass(commandIDPtr);
    jmethodID mid = env->GetMethodID(clazz, "getValue", "()I");
    jint commandIDPtrIntValue = env->CallIntMethod(commandIDPtr, mid);

    uint64_t commandIDPtrUint64Value, *commandIDPtrUint64;
    commandIDPtrUint64Value=uint64_t(commandIDPtrIntValue);
    commandIDPtrUint64=&commandIDPtrUint64Value;

Ricavo prima il valore intero assegnato alla variabile dell'oggetto IntReference ricevuto,
poi creo la variabile richiesta ed il puntatore ad essa da passare.

########################################################
### Accedere campi e metodi java da codice nativo
########################################################

Ottenuto un riferimento locale ad un oggetto Java, per ivocarne un metodo occorre:

1) Definire un oggetto JClass che rappresenti la classe cui appartiene il metodo
2) Ricavare la signature del metodo
3) Ricavare l'ID del metodo
4) Invocare il metodo

Esp)
Richiamo il metodo "int getValue()" della classe IntReference:

    jclass clazz = env->GetObjectClass(dev_id_ptr);
    jmethodID mid = env->GetMethodID(clazz, "getValue", "()I");
    jint value = env->CallIntMethod(dev_id_ptr, mid);

Per gli altri metodi vedi:
http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/functions.html


