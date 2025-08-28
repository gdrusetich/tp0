#include "client.h"
#include <readline/readline.h>
int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	char* linea_ingresada;

	t_log* logger;
	t_config* config;

	

	/* ---------------- LOGGING ---------------- */
	//Iniciar logger con log_create() y lo probamos
	logger = iniciar_logger();
	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	//Iniciamos la configuracion con config_create y detectamos error
	config = iniciar_config();
	if(config == NULL)
	{
		log_info(logger, "No se inició la configuración");
	} else {
		log_info(logger, "Se inició la configuración");
	}

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	
	valor = config_get_string_value(config,"CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	
	// Loggeamos el valor de config
	log_info(logger, "La clave es: %s", valor);
	log_info(logger, "El IP es: %s", ip);
	log_info(logger, "El Puerto es: %s", puerto);
	
	/* ---------------- LEER DE CONSOLA ---------------- */
	//Metimos esta parte dentro de la funcion paquete()
	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor y prob amos errores
	conexion = crear_conexion(ip, puerto);
	if(conexion < 0)
	{
		log_error(logger,"No se pudo crear la conexion");
		terminar_programa(conexion, logger, config);
		exit(1);
	}
	log_info(logger, "Se creó una conexión al servidor!");

	// Enviamos el handshake al servidor para comprobar la comunicación.
	enviar_mensaje(valor, conexion);
	log_info(logger, "Se envio al servidor la clave: %s", valor);

	// Armamos el paquete con líneas ingresadas, lo enviamos, y eliminamos
	paquete(conexion);
	log_info(logger, "Se enviaron las lineas ingresadas como paquete al servidor");

	terminar_programa(conexion, logger, config);
	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente (No tengo idea si había que hacer algo más)
	
	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	return log_create("logger.log","logger", true, LOG_LEVEL_INFO);
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	return config_create("cliente.config");
}


//Usamos este modelo, para realizar lo mismo dentro de la funcion paquete()
void leer_consola(t_log* logger)
{
	char* linea_ingresada;

	// La primera te la dejo de yapa
	linea_ingresada = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(linea_ingresada != NULL && strlen(linea_ingresada)>0)
	{
		log_info(logger, linea_ingresada);
	// ¡No te olvides de liberar las lineas antes de regresar!
		free(linea_ingresada);
		linea_ingresada = readline("> ");
	}
	free(linea_ingresada);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Agregamos cada línea leída a un paquete. Si la línea es vacía, se envia el paquete y luego se libera.
	leido = readline("> ");
	while(leido != NULL && strlen(leido)>0)
	{
		agregar_a_paquete(paquete,leido, strlen(leido)+1);
		free(leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}

