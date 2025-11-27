using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
/*
LO QUE PIDE DE AVANCE:

1. el servidor debe estar configurado para enviar notificaciones de 
conexión y desconexión a todos los usuarios de la sala.(Esto debe estar imlemetado mas adelante)

2. el diseño debe estar claro y se debe proporcionar una explicación detallada del flujo de 
notificaciones.

3.incluido un diagrama que muestre cómo las notificaciones son enviadas y gestionadas en el servidor. 

DETALLES DE FUNCIONAMIENTO REQUERIDOS, ESTO LO MENCIONA LE DOCUMENTO:

1. Notificaciones de Conexión y Desconexión: El servidor debe enviar una 
notificación a todos los clientes cuando un nuevo usuario se conecta o se 
desconecta de la sala de chat. 

este mensaje debe incluir el nombre, si se ha conenctado o desconectado, y la hora del evento.

2. Notificación de Inactividad: El servidor debe detectar la inactividad de 
los usuarios. Si un usuario no envía ningún mensaje durante un período 
de tiempo determinado, el servidor envía una 
notificación indicando que el usuario está inactivo. Si el usuario vuelve a 
enviar un mensaje, el estado de inactividad debe actualizarse. 

3.Comando de Activación/Desactivación de Notificaciones: Los usuarios 
deben poder controlar si quieren recibir notificaciones mediante el 
comando /notificaciones <on|off>. Esto les permite optar por una 
experiencia de chat más tranquila si no desean ser notificados de cada 
evento. 

*/
public class Servidor
{
    private static ConcurrentDictionary<string, Usuario> usuarios = new ConcurrentDictionary<string, Usuario>();

}