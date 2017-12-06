using System.Collections.Generic;

namespace Tars.Csharp.Codecs.Tup
{
    public class RequestPacket
    {
        public short Version { get; set; }

        public byte PacketType { get; set; }

        public int MessageType { get; set; }

        public int RequestId { get; set; }

        public string ServantName { get; set; }

        public string FuncName { get; set; }

        public byte[] Buffer { get; set; }

        public int Timeout { get; set; }

        public IDictionary<string, string> Context { get; set; } = new Dictionary<string, string>();

        public IDictionary<string, string> Status { get; set; } = new Dictionary<string, string>();

        public int Ret { get; set; }

        public string ResultDesc { get; set; }

        public RequestPacket CreateResponse()
        {
            return new RequestPacket()
            {
                Version = Version,
                MessageType = MessageType,
                RequestId = RequestId,
                ServantName = ServantName,
                FuncName = FuncName,
                Timeout = Timeout,
                Context = new Dictionary<string, string>(),
                Status = new Dictionary<string, string>()
            };
        }

        public void SetRetToStatus()
        {
            Status.Add(Const.StatusResultCode, Ret.ToString());
            Status.Add(Const.StatusResultDesc, ResultDesc ?? "");
        }

        public void GetRetToStatus()
        {
            if (Status.TryGetValue(Const.StatusResultCode, out string code))
            {
                int.TryParse(code, out int ret);
                Ret = ret;
            }
            if (Status.TryGetValue(Const.StatusResultDesc, out string retStatus))
            {
                ResultDesc = retStatus;
            }
        }
    }
}