using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW07 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();
        }

        private enum QUERY_TYPE {
            JSON, XML
        }

        /// <summary>
        /// 查询按钮点击事件(Json)
        /// </summary>
        private void JsonQueryClick(object sender, RoutedEventArgs e) {
            /* 基础url */
            string baseUrl = "http://fanyi.youdao.com/openapi.do";
            /* 必要的前缀, 其中包含apikey */
            string getPrefix = "?keyfrom=UWP-Translator&key=1324263559&type=data&doctype=json&version=1.1&q=";

            if (string.IsNullOrEmpty(DictContent.Text)) { /* 检查输入是否为空 */
                var i = new MessageDialog("查询内容不能为空！").ShowAsync();
            } else {
                /* 调用请求函数 */
                string param = DictContent.Text;
                GetRequest(QUERY_TYPE.JSON, baseUrl + getPrefix, param);
            }
        }

        /// <summary>
        /// 查询按钮点击事件(Xml)
        /// </summary>
        private void XmlQueryClick(object sender, RoutedEventArgs e) {
            /* 基础url */
            string baseUrl = "http://fanyi.youdao.com/openapi.do";
            /* 必要的前缀, 其中包含apikey */
            string getPrefix = "?keyfrom=UWP-Translator&key=1324263559&type=data&doctype=xml&version=1.1&q=";

            if (string.IsNullOrEmpty(DictContent.Text)) { /* 检查输入是否为空 */
                var i = new MessageDialog("查询内容不能为空！").ShowAsync();
            } else {
                /* 调用请求函数 */
                string param = DictContent.Text;
                GetRequest(QUERY_TYPE.XML, baseUrl + getPrefix, param);
            }
        }

        /// <summary>
        /// 发送HTTP请求
        /// </summary>
        /// <param name="qtype">查询类型</param>
        /// <param name="url">请求的URL</param>
        /// <param name="param">请求的参数</param>
        private async void GetRequest(QUERY_TYPE qtype, string url, string param) {
            /* Note: the URI constructor will throw an exception
             * if the string passed is not a valid URI */
            var uri = new Uri(url + param);
            var httpClient = new HttpClient();

            /* Always catch network exceptions for async methods */
            try {
                var response = await httpClient.GetStringAsync(uri); /* 获取结果 */
                if (qtype == QUERY_TYPE.JSON) {
                    DictResult.Text = jsonReader(response); /* 处理json */
                } else if (qtype == QUERY_TYPE.XML) {
                    DictResult.Text = xmlReader(response); /* 处理xml */
                }
            } catch (Exception e) {
                /* Details in ex.Message and ex.HResult. */
                DictResult.Text = e.ToString();
            }

            httpClient.Dispose(); /* 释放资源 */
        }

        /// <summary>
        /// 解析Json
        /// </summary>
        /// <param name="json">待处理的json字符串</param>
        private string jsonReader(string json) {
            string value = "解释："; /* 返回值 */

            /* 反序列化 */
            JObject jObject = (JObject)JsonConvert.DeserializeObject(json);

            /* 由于解释可能为空(翻译不会为空), 加上判断条件 */
            if (jObject.GetValue("basic") != null) {
                JArray basicExplain = (JArray)jObject["basic"]["explains"];
                /* 遍历 */
                foreach (JToken item in basicExplain) {
                    value += "\t" + item + ".\n";
                }
            } else {
                value += "\t" + "没有查询到释义！\n";
            }

            /* 翻译部分 */
            value += "\n翻译：";
            JArray translation = (JArray)jObject["translation"];
            /* 遍历 */
            foreach (JToken item in translation) {
                value += "\t" + item + ".\n";
            }

            return value;
        }

        /// <summary>
        /// 解析Xml
        /// </summary>
        /// <param name="xml">待处理的xml字符串</param>
        private string xmlReader(string xml) {
            string value = "解释："; /* 返回值 */

            /* 声明并载入XmlDoc */
            XmlDocument xDoc = new XmlDocument();
            xDoc.LoadXml(xml);

            XmlElement root = xDoc.DocumentElement; /* 设置根元素 */
            XmlNodeList listNodes = null; /* 声明节点列表 */

            /* 解释部分 */
            listNodes = root.SelectNodes("/youdao-fanyi/basic/explains/ex");
            if (listNodes.Length != 0) {
                /* 遍历 */
                foreach (XmlElement node in listNodes) {
                    value += "\t" + node.InnerText + ".\n";
                }
            } else {
                value += "\t" + "没有查询到释义！\n";
            }

            /* 翻译部分 */
            value += "\n翻译：";
            listNodes = root.SelectNodes("/youdao-fanyi/translation/paragraph");
            /* 遍历 */
            foreach (XmlElement node in listNodes) {
                value += "\t" + node.InnerText + ".\n";
            }

            return value;
        }
    }
}
