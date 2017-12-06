namespace Tars.Csharp.Codecs
{
    public abstract class TarsStruct
    {
        public abstract void WriteTo(TarsOutputStream tarsOutputStream);

        public abstract void ReadFrom(TarsInputStream tarsInputStream);
    }
}