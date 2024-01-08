import express from 'express';
import bodyParser from 'body-parser';
import dotenv from 'dotenv';
import OpenAI from 'openai';
import axios from 'axios';

dotenv.config();

const app = express();
const openai = new OpenAI();
const PORT = 20001;

openai.api_key = process.env.OPENAI_API_KEY;
const chat = async (msg) => {
    const completion = await openai.chat.completions.create({
        messages: [{ role: "system", content: "以繁體中文回覆訊息,不超過100字." },
            { role: "user", content: `這是每隔一秒讀取的心率和血氧數值,資料量等同秒數,量測者為坐著.分析數值,依照現實忽略誤差明顯的值.回覆平均血氧,心率,數據紀錄時長,對數值的健康評估.不須有額外的回覆.${msg}`}],
        model: "gpt-3.5-turbo",
    });
    const replyMsg = completion.choices[0].message.content;
    return replyMsg;
}

app.use(bodyParser.json());

app.post('/receive-data', async (req, res) => {
    const msg = JSON.stringify(req.body);
    const replyMsg = await chat(msg);
    console.log(msg);
    console.log(replyMsg);

    const fakemsg = "{暫不顯示}";

    const webhookUrl = process.env.MAKE_WEBHOOK;
    try {
        const response = await axios.get(`${webhookUrl}?replymsg=${encodeURIComponent(replyMsg)}&msg=${encodeURIComponent(fakemsg)}`);
        console.log(response.data);
    } catch (error) {
        console.error('Error sending GET request:', error);
    }

    res.status(200).send('Data received successfully');
});

app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
});

