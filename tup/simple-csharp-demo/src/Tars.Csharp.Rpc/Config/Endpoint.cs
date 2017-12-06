using System;
using System.Text;
using System.Linq;
using System.Text.RegularExpressions;

namespace Tars.Csharp.Rpc.Config
{
    public class Endpoint
    {
        public static readonly Endpoint VOID_ENDPOINT = new Endpoint("tcp", "0.0.0.0", 0, 0, 0, 0, null);
        private int hash;
        public string Type { get; set; }
        public string Host { get; set; }
        public int Port { get; set; }
        public int Timeout { get; set; }
        public int Grid { get; set; }
        public int Qos { get; set; }
        public string SetDivision { get; set; }

        public Endpoint(string type, string host, int port, int timeout, int grid, int qos, string setDivision)
        {
            Type = type;
            Host = host;
            Port = port;
            Timeout = timeout;
            Grid = grid;
            Qos = qos;
            SetDivision = setDivision;
        }

        public override int GetHashCode()
        {
            int h = hash;
            if (h == 0)
            {
                h = 1;
                h = 31 * h + Type.GetHashCode();
                h = 31 * h + Host.GetHashCode();
                h = 31 * h + Port;
                hash = h;
            }
            return h;
        }

        public override bool Equals(object obj)
        {
            if (this == obj) return true;
            if (obj == null) return false;
            if (GetType() != obj.GetType()) return false;
            Endpoint other = (Endpoint)obj;
            if (Host == null)
            {
                if (other.Host != null) return false;
            }
            else if (!Host.Equals(other.Host)) return false;
            if (Port != other.Port) return false;
            if (Type != other.Type) return false;
            return true;
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(Type.ToLower());
            sb.Append(" -h ").Append(Host);
            sb.Append(" -p ").Append(Port);
            if (Timeout > 0)
            {
                sb.Append(" -t ").Append(Timeout);
            }
            if (Grid > 0)
            {
                sb.Append(" -g ").Append(Grid);
            }
            if (Qos > 0)
            {
                sb.Append(" -q ").Append(Qos);
            }
            if (SetDivision != null && SetDivision.Trim().Length != 0)
            {
                sb.Append(" -s ").Append(SetDivision);
            }
            return sb.ToString();
        }

        public static Endpoint ParseString(string node)
        {
            string type = null;
            string host = null;
            int port = 0;
            int timeout = 3000;
            int grid = 0;
            int qos = 0;
            string setDivision = null;

            string[] fields = Regex.Split(node, "\\s+");
            if ("tcp".Equals(fields[0], StringComparison.OrdinalIgnoreCase))
            {
                type = "tcp";
            }
            else if ("udp".Equals(fields[0], StringComparison.OrdinalIgnoreCase))
            {
                type = "udp";
            }
            else
            {
                return null;
            }

            int i = 0;
            while (i < fields.Length)
            {
                if ("-h".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    host = fields[i + 1];
                    i += 2;
                }
                else if ("-p".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    port = int.Parse(fields[i + 1]);
                    i += 2;
                }
                else if ("-t".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    timeout = int.Parse(fields[i + 1]);
                    i += 2;
                }
                else if ("-g".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    grid = int.Parse(fields[i + 1]);
                    i += 2;
                }
                else if ("-q".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    qos = int.Parse(fields[i + 1]);
                    i += 2;
                }
                else if ("-s".Equals(fields[i]) && i + 1 < fields.Length)
                {
                    setDivision = fields[i + 1];
                    i += 2;
                }
                else
                {
                    i++;
                }
            }

            if (host == null || host.Length == 0)
            {
                return null;
            }
            else if (host.Equals("*"))
            {
                host = "0.0.0.0";
            }

            if (port < 0 || port > 65535)
            {
                return null;
            }

            if (timeout <= 0)
            {
                return null;
            }

            if (grid < 0)
            {
                return null;
            }
            return new Endpoint(type, host, port, timeout, grid, qos, setDivision);
        }
    }
}