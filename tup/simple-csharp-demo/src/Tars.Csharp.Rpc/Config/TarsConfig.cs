using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Tars.Csharp.Rpc.Config
{
    public class TarsConfig
    {
        private const string TAG_STARTER = "<";
        private const string TAG_OVER = "</";
        private const string TAG_ENDER = "/>";
        private const string TAG_CLOSE = ">";
        private const string ATTR_FLAGF = "=";
        private const string COMMENT = "#";

        private Dictionary<string, List<string>> subTags = new Dictionary<string, List<string>>();
        private Dictionary<string, List<string>> subKeys = new Dictionary<string, List<string>>();
        private Dictionary<string, string> values = new Dictionary<string, string>();
        private Dictionary<string, string> escapedValues = new Dictionary<string, string>();

        private TarsConfig(string[] lines)
        {
            LinkedList<string> stack = new LinkedList<string>();
            stack.AddLast("");
            subTags.Add("", new List<string>());
            subKeys.Add("", new List<string>());
            for (int i = 0; i < lines.Length; ++i)
            {
                string line = lines[i].Trim();
                if (line.Length <= 0 || line.StartsWith(COMMENT))
                {
                    continue;
                }
                if (line.StartsWith(TAG_OVER) && line.EndsWith(TAG_CLOSE))
                {
                    string name = line.Substring(TAG_OVER.Length, line.Length - TAG_CLOSE.Length - TAG_OVER.Length);
                    string path = stack.Last.Value;
                    if (!path.EndsWith("/" + name))
                    {
                        throw new Exception("Invalid tag end : " + name + " at line no " + i);
                    }
                    stack.RemoveLast();
                }
                else if (line.StartsWith(TAG_STARTER) && line.EndsWith(TAG_ENDER))
                {
                    string name = line.Substring(TAG_STARTER.Length, line.Length - TAG_ENDER.Length - TAG_STARTER.Length);
                    string path = stack.Last.Value;
                    string newPath = path + "/" + name;
                    subTags[path].Add(name);
                    if (!subTags.ContainsKey(newPath))
                    {
                        subTags.Add(newPath, new List<string>());
                    }
                    if (!subKeys.ContainsKey(newPath))
                    {
                        subKeys.Add(newPath, new List<string>());
                    }
                }
                else if (line.StartsWith(TAG_STARTER) && line.EndsWith(TAG_CLOSE))
                {
                    string name = line.Substring(TAG_STARTER.Length, line.Length - TAG_CLOSE.Length - TAG_STARTER.Length);
                    string path = stack.Last.Value;
                    string newPath = path + "/" + name;
                    subTags[path].Add(name);
                    if (!subTags.ContainsKey(newPath))
                    {
                        subTags.Add(newPath, new List<string>());
                    }
                    if (!subKeys.ContainsKey(newPath))
                    {
                        subKeys.Add(newPath, new List<string>());
                    }
                    stack.AddLast(newPath);
                }
                else if (line.StartsWith(TAG_STARTER) || line.EndsWith(TAG_CLOSE))
                {
                    throw new Exception("Invalid line : " + line + " at line no " + i);
                }
                else
                {
                    string key;
                    string value;
                    int idx = line.IndexOf(ATTR_FLAGF);
                    if (idx < 0)
                    {
                        key = line.Trim();
                        value = "";
                    }
                    else
                    {
                        key = line.Substring(0, idx).Trim();
                        value = line.Substring(idx + ATTR_FLAGF.Length).Trim();
                    }

                    string path = stack.Last.Value;
                    string newPath = path + "<" + key + ">";

                    subKeys[path].Add(key);
                    values.Add(newPath, value);
                    escapedValues.Add(newPath, UnEscape(value));
                }
            }
        }

        private string UnEscape(string value)
        {
            return value.Replace("&amp;", "&").Replace("&nbsp;", " ").Replace("&lt;", "<").Replace("&gt;", ">").Replace("&quot;", "\"");
        }

        public string Get(string path, bool escape)
        {
            return escape ? (escapedValues.TryGetValue(path, out string result) ? result : string.Empty) 
                : (values.TryGetValue(path, out result) ? result : string.Empty);
        }

        public string Get(string path)
        {
            return Get(path, false);
        }

        public string Get(string path, string defaultValue)
        {
            string value = Get(path, false);
            return value ?? defaultValue;
        }

        public int GetInt(string path, int defaultValue)
        {
            string str = Get(path);
            return int.TryParse(str, out int result) ? result : defaultValue;
        }

        public bool GetBool(string path, bool defaultValue)
        {
            string str = Get(path);
            return bool.TryParse(str, out bool result) ? result : defaultValue;
        }

        public List<string> GetSubTags(string path)
        {
            return subTags[path];
        }

        public List<string> GetSubKeys(string path)
        {
            return subKeys[path];
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            ToStr(sb, "", "");
            return sb.ToString();
        }

        private void ToStr(StringBuilder sb, string indent, string path)
        {
            foreach (string key in subKeys[path])
            {
                string value = values[path + "<" + key + ">"];
                sb.Append(indent).Append(key).Append(ATTR_FLAGF).Append(value).Append("\n");
            }
            foreach (string name in subTags[path])
            {
                string newPath = path + "/" + name;
                List<string> subTags = this.subTags[newPath];
                List<string> subKeys = this.subKeys[newPath];
                if (subTags.Count == 0 && subKeys.Count == 0)
                {
                    sb.Append(indent).Append(TAG_STARTER).Append(name).Append(TAG_ENDER).Append("\n");
                }
                else
                {
                    sb.Append(indent).Append(TAG_STARTER).Append(name).Append(TAG_CLOSE).Append("\n");
                    ToStr(sb, indent + "  ", newPath);
                    sb.Append(indent).Append(TAG_OVER).Append(name).Append(TAG_CLOSE).Append("\n");
                }
            }
        }

        public static TarsConfig Parse(string content)
        {
            if (content == null)
            {
                throw new ArgumentNullException("content");
            }
            return new TarsConfig(content.Split('\n'));
        }

        public static TarsConfig ParseFile(string v)
        {
            return new TarsConfig(File.ReadAllLines(v));
        }
    }
}