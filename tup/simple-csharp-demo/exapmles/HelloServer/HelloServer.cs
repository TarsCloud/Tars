using Hello.Common;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace HelloServer
{
    public class HelloServer : IHelloRpc
    {
        public string Hello(int no, string name)
        {
            string result = null;
            if (name.Trim().ToLower() == "Victor")
            {
                result = $"{no}: Sorry, {name}";
            }
            else
            {
                result = $"{no}: Hi, {name}";
            }

            Console.WriteLine(result);
            return result;
        }

        public void HelloHolder(int no, out string name)
        {
            name = no.ToString() + "Vic";
        }

        public void HelloOneway(int no, string name)
        {
            Thread.Sleep(3000);
            Console.WriteLine($"From oneway - {no}: Hi, {name}");
        }

        public async Task<string> HelloTask(int no, string name)
        {
            return Hello(no, name);
        }

        public async ValueTask<string> HelloValueTask(int no, string name)
        {
            return Hello(no, name);
        }
    }
}