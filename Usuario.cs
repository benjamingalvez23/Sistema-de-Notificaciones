using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
/**/
public class Usuario
{
    public Socket Socket { get; set; }
    public string Nombre { get; set; }

    public Usuario(Socket socket, string nombre)
    {
        Socket = socket;
        Nombre = nombre;
    }
}