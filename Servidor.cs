using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
/*
1. el servidor debe estar configurado para enviar notificaciones de 
conexión y desconexión a todos los usuarios de la sala.(Esto debe estar imlemetado mas adelante)

2. el diseño debe estar claro y se debe proporcionar una explicación detallada del flujo de 
notificaciones.

3.incluido un diagrama que muestre cómo las notificaciones son enviadas y gestionadas en el servidor. 
*/
public class Servidor
{
    private static ConcurrentDictionary<string, Usuario> usuarios = new ConcurrentDictionary<string, Usuario>();

}