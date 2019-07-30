load('api_sys.js');
load('api_common.js');

let COLOR_TOOLS = {
	name: "COLOR_TOOLS",

	colorHexToRgb: function (hex) {
		hex = TOOLS.replaceString(hex, '#', '', true);
		let r,g,b;
		let rgb = [];
		if (hex.length === 3) {
			r = hex.slice(0, 1);
			g = hex.slice(1, 2);
			b = hex.slice(2, 3);
			r += r;
			g += g;
			b += b;
		} else if (hex.length === 6) {
			r = hex.slice(0, 2);
			g = hex.slice(2, 4);
			b = hex.slice(4, 6);
		} else {
			return [];
		}
		rgb.push(JSON.parse('0x' + r));
		rgb.push(JSON.parse('0x' + g));
		rgb.push(JSON.parse('0x' + b));
		return rgb;
	}
}
