/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>

#include <cstdio>
#include <cstring>

#include <crisscross/core_io_reader.h>
#include <crisscross/tiger.h>

/*
 * This code was adapted from GnuPG and is licensed under the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the license, or (at your option) any later version.
 * Please see COPYING for the license regarding this file
 *
 * Some functions have been changed or removed from the GnuPG version.
 * See comments for details.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

namespace
{
	uint64_t sbox1[256] = {
		0x02aab17cf7e90c5eULL /*    0 */, 0xac424b03e243a8ecULL /*    1 */,
		0x72cd5be30dd5fcd3ULL /*    2 */, 0x6d019b93f6f97f3aULL /*    3 */,
		0xcd9978ffd21f9193ULL /*    4 */, 0x7573a1c9708029e2ULL /*    5 */,
		0xb164326b922a83c3ULL /*    6 */, 0x46883eee04915870ULL /*    7 */,
		0xeaace3057103ece6ULL /*    8 */, 0xc54169b808a3535cULL /*    9 */,
		0x4ce754918ddec47cULL /*   10 */, 0x0aa2f4dfdc0df40cULL /*   11 */,
		0x10b76f18a74dbefaULL /*   12 */, 0xc6ccb6235ad1ab6aULL /*   13 */,
		0x13726121572fe2ffULL /*   14 */, 0x1a488c6f199d921eULL /*   15 */,
		0x4bc9f9f4da0007caULL /*   16 */, 0x26f5e6f6e85241c7ULL /*   17 */,
		0x859079dbea5947b6ULL /*   18 */, 0x4f1885c5c99e8c92ULL /*   19 */,
		0xd78e761ea96f864bULL /*   20 */, 0x8e36428c52b5c17dULL /*   21 */,
		0x69cf6827373063c1ULL /*   22 */, 0xb607c93d9bb4c56eULL /*   23 */,
		0x7d820e760e76b5eaULL /*   24 */, 0x645c9cc6f07fdc42ULL /*   25 */,
		0xbf38a078243342e0ULL /*   26 */, 0x5f6b343c9d2e7d04ULL /*   27 */,
		0xf2c28aeb600b0ec6ULL /*   28 */, 0x6c0ed85f7254bcacULL /*   29 */,
		0x71592281a4db4fe5ULL /*   30 */, 0x1967fa69ce0fed9fULL /*   31 */,
		0xfd5293f8b96545dbULL /*   32 */, 0xc879e9d7f2a7600bULL /*   33 */,
		0x860248920193194eULL /*   34 */, 0xa4f9533b2d9cc0b3ULL /*   35 */,
		0x9053836c15957613ULL /*   36 */, 0xdb6dcf8afc357bf1ULL /*   37 */,
		0x18beea7a7a370f57ULL /*   38 */, 0x037117ca50b99066ULL /*   39 */,
		0x6ab30a9774424a35ULL /*   40 */, 0xf4e92f02e325249bULL /*   41 */,
		0x7739db07061ccae1ULL /*   42 */, 0xd8f3b49ceca42a05ULL /*   43 */,
		0xbd56be3f51382f73ULL /*   44 */, 0x45faed5843b0bb28ULL /*   45 */,
		0x1c813d5c11bf1f83ULL /*   46 */, 0x8af0e4b6d75fa169ULL /*   47 */,
		0x33ee18a487ad9999ULL /*   48 */, 0x3c26e8eab1c94410ULL /*   49 */,
		0xb510102bc0a822f9ULL /*   50 */, 0x141eef310ce6123bULL /*   51 */,
		0xfc65b90059ddb154ULL /*   52 */, 0xe0158640c5e0e607ULL /*   53 */,
		0x884e079826c3a3cfULL /*   54 */, 0x930d0d9523c535fdULL /*   55 */,
		0x35638d754e9a2b00ULL /*   56 */, 0x4085fccf40469dd5ULL /*   57 */,
		0xc4b17ad28be23a4cULL /*   58 */, 0xcab2f0fc6a3e6a2eULL /*   59 */,
		0x2860971a6b943fcdULL /*   60 */, 0x3dde6ee212e30446ULL /*   61 */,
		0x6222f32ae01765aeULL /*   62 */, 0x5d550bb5478308feULL /*   63 */,
		0xa9efa98da0eda22aULL /*   64 */, 0xc351a71686c40da7ULL /*   65 */,
		0x1105586d9c867c84ULL /*   66 */, 0xdcffee85fda22853ULL /*   67 */,
		0xccfbd0262c5eef76ULL /*   68 */, 0xbaf294cb8990d201ULL /*   69 */,
		0xe69464f52afad975ULL /*   70 */, 0x94b013afdf133e14ULL /*   71 */,
		0x06a7d1a32823c958ULL /*   72 */, 0x6f95fe5130f61119ULL /*   73 */,
		0xd92ab34e462c06c0ULL /*   74 */, 0xed7bde33887c71d2ULL /*   75 */,
		0x79746d6e6518393eULL /*   76 */, 0x5ba419385d713329ULL /*   77 */,
		0x7c1ba6b948a97564ULL /*   78 */, 0x31987c197bfdac67ULL /*   79 */,
		0xde6c23c44b053d02ULL /*   80 */, 0x581c49fed002d64dULL /*   81 */,
		0xdd474d6338261571ULL /*   82 */, 0xaa4546c3e473d062ULL /*   83 */,
		0x928fce349455f860ULL /*   84 */, 0x48161bbacaab94d9ULL /*   85 */,
		0x63912430770e6f68ULL /*   86 */, 0x6ec8a5e602c6641cULL /*   87 */,
		0x87282515337ddd2bULL /*   88 */, 0x2cda6b42034b701bULL /*   89 */,
		0xb03d37c181cb096dULL /*   90 */, 0xe108438266c71c6fULL /*   91 */,
		0x2b3180c7eb51b255ULL /*   92 */, 0xdf92b82f96c08bbcULL /*   93 */,
		0x5c68c8c0a632f3baULL /*   94 */, 0x5504cc861c3d0556ULL /*   95 */,
		0xabbfa4e55fb26b8fULL /*   96 */, 0x41848b0ab3baceb4ULL /*   97 */,
		0xb334a273aa445d32ULL /*   98 */, 0xbca696f0a85ad881ULL /*   99 */,
		0x24f6ec65b528d56cULL /*  100 */, 0x0ce1512e90f4524aULL /*  101 */,
		0x4e9dd79d5506d35aULL /*  102 */, 0x258905fac6ce9779ULL /*  103 */,
		0x2019295b3e109b33ULL /*  104 */, 0xf8a9478b73a054ccULL /*  105 */,
		0x2924f2f934417eb0ULL /*  106 */, 0x3993357d536d1bc4ULL /*  107 */,
		0x38a81ac21db6ff8bULL /*  108 */, 0x47c4fbf17d6016bfULL /*  109 */,
		0x1e0faadd7667e3f5ULL /*  110 */, 0x7abcff62938beb96ULL /*  111 */,
		0xa78dad948fc179c9ULL /*  112 */, 0x8f1f98b72911e50dULL /*  113 */,
		0x61e48eae27121a91ULL /*  114 */, 0x4d62f7ad31859808ULL /*  115 */,
		0xeceba345ef5ceaebULL /*  116 */, 0xf5ceb25ebc9684ceULL /*  117 */,
		0xf633e20cb7f76221ULL /*  118 */, 0xa32cdf06ab8293e4ULL /*  119 */,
		0x985a202ca5ee2ca4ULL /*  120 */, 0xcf0b8447cc8a8fb1ULL /*  121 */,
		0x9f765244979859a3ULL /*  122 */, 0xa8d516b1a1240017ULL /*  123 */,
		0x0bd7ba3ebb5dc726ULL /*  124 */, 0xe54bca55b86adb39ULL /*  125 */,
		0x1d7a3afd6c478063ULL /*  126 */, 0x519ec608e7669eddULL /*  127 */,
		0x0e5715a2d149aa23ULL /*  128 */, 0x177d4571848ff194ULL /*  129 */,
		0xeeb55f3241014c22ULL /*  130 */, 0x0f5e5ca13a6e2ec2ULL /*  131 */,
		0x8029927b75f5c361ULL /*  132 */, 0xad139fabc3d6e436ULL /*  133 */,
		0x0d5df1a94ccf402fULL /*  134 */, 0x3e8bd948bea5dfc8ULL /*  135 */,
		0xa5a0d357bd3ff77eULL /*  136 */, 0xa2d12e251f74f645ULL /*  137 */,
		0x66fd9e525e81a082ULL /*  138 */, 0x2e0c90ce7f687a49ULL /*  139 */,
		0xc2e8bcbeba973bc5ULL /*  140 */, 0x000001bce509745fULL /*  141 */,
		0x423777bbe6dab3d6ULL /*  142 */, 0xd1661c7eaef06eb5ULL /*  143 */,
		0xa1781f354daacfd8ULL /*  144 */, 0x2d11284a2b16affcULL /*  145 */,
		0xf1fc4f67fa891d1fULL /*  146 */, 0x73ecc25dcb920adaULL /*  147 */,
		0xae610c22c2a12651ULL /*  148 */, 0x96e0a810d356b78aULL /*  149 */,
		0x5a9a381f2fe7870fULL /*  150 */, 0xd5ad62ede94e5530ULL /*  151 */,
		0xd225e5e8368d1427ULL /*  152 */, 0x65977b70c7af4631ULL /*  153 */,
		0x99f889b2de39d74fULL /*  154 */, 0x233f30bf54e1d143ULL /*  155 */,
		0x9a9675d3d9a63c97ULL /*  156 */, 0x5470554ff334f9a8ULL /*  157 */,
		0x166acb744a4f5688ULL /*  158 */, 0x70c74caab2e4aeadULL /*  159 */,
		0xf0d091646f294d12ULL /*  160 */, 0x57b82a89684031d1ULL /*  161 */,
		0xefd95a5a61be0b6bULL /*  162 */, 0x2fbd12e969f2f29aULL /*  163 */,
		0x9bd37013feff9fe8ULL /*  164 */, 0x3f9b0404d6085a06ULL /*  165 */,
		0x4940c1f3166cfe15ULL /*  166 */, 0x09542c4dcdf3defbULL /*  167 */,
		0xb4c5218385cd5ce3ULL /*  168 */, 0xc935b7dc4462a641ULL /*  169 */,
		0x3417f8a68ed3b63fULL /*  170 */, 0xb80959295b215b40ULL /*  171 */,
		0xf99cdaef3b8c8572ULL /*  172 */, 0x018c0614f8fcb95dULL /*  173 */,
		0x1b14accd1a3acdf3ULL /*  174 */, 0x84d471f200bb732dULL /*  175 */,
		0xc1a3110e95e8da16ULL /*  176 */, 0x430a7220bf1a82b8ULL /*  177 */,
		0xb77e090d39df210eULL /*  178 */, 0x5ef4bd9f3cd05e9dULL /*  179 */,
		0x9d4ff6da7e57a444ULL /*  180 */, 0xda1d60e183d4a5f8ULL /*  181 */,
		0xb287c38417998e47ULL /*  182 */, 0xfe3edc121bb31886ULL /*  183 */,
		0xc7fe3ccc980ccbefULL /*  184 */, 0xe46fb590189bfd03ULL /*  185 */,
		0x3732fd469a4c57dcULL /*  186 */, 0x7ef700a07cf1ad65ULL /*  187 */,
		0x59c64468a31d8859ULL /*  188 */, 0x762fb0b4d45b61f6ULL /*  189 */,
		0x155baed099047718ULL /*  190 */, 0x68755e4c3d50baa6ULL /*  191 */,
		0xe9214e7f22d8b4dfULL /*  192 */, 0x2addbf532eac95f4ULL /*  193 */,
		0x32ae3909b4bd0109ULL /*  194 */, 0x834df537b08e3450ULL /*  195 */,
		0xfa209da84220728dULL /*  196 */, 0x9e691d9b9efe23f7ULL /*  197 */,
		0x0446d288c4ae8d7fULL /*  198 */, 0x7b4cc524e169785bULL /*  199 */,
		0x21d87f0135ca1385ULL /*  200 */, 0xcebb400f137b8aa5ULL /*  201 */,
		0x272e2b66580796beULL /*  202 */, 0x3612264125c2b0deULL /*  203 */,
		0x057702bdad1efbb2ULL /*  204 */, 0xd4babb8eacf84be9ULL /*  205 */,
		0x91583139641bc67bULL /*  206 */, 0x8bdc2de08036e024ULL /*  207 */,
		0x603c8156f49f68edULL /*  208 */, 0xf7d236f7dbef5111ULL /*  209 */,
		0x9727c4598ad21e80ULL /*  210 */, 0xa08a0896670a5fd7ULL /*  211 */,
		0xcb4a8f4309eba9cbULL /*  212 */, 0x81af564b0f7036a1ULL /*  213 */,
		0xc0b99aa778199abdULL /*  214 */, 0x959f1ec83fc8e952ULL /*  215 */,
		0x8c505077794a81b9ULL /*  216 */, 0x3acaaf8f056338f0ULL /*  217 */,
		0x07b43f50627a6778ULL /*  218 */, 0x4a44ab49f5eccc77ULL /*  219 */,
		0x3bc3d6e4b679ee98ULL /*  220 */, 0x9cc0d4d1cf14108cULL /*  221 */,
		0x4406c00b206bc8a0ULL /*  222 */, 0x82a18854c8d72d89ULL /*  223 */,
		0x67e366b35c3c432cULL /*  224 */, 0xb923dd61102b37f2ULL /*  225 */,
		0x56ab2779d884271dULL /*  226 */, 0xbe83e1b0ff1525afULL /*  227 */,
		0xfb7c65d4217e49a9ULL /*  228 */, 0x6bdbe0e76d48e7d4ULL /*  229 */,
		0x08df828745d9179eULL /*  230 */, 0x22ea6a9add53bd34ULL /*  231 */,
		0xe36e141c5622200aULL /*  232 */, 0x7f805d1b8cb750eeULL /*  233 */,
		0xafe5c7a59f58e837ULL /*  234 */, 0xe27f996a4fb1c23cULL /*  235 */,
		0xd3867dfb0775f0d0ULL /*  236 */, 0xd0e673de6e88891aULL /*  237 */,
		0x123aeb9eafb86c25ULL /*  238 */, 0x30f1d5d5c145b895ULL /*  239 */,
		0xbb434a2dee7269e7ULL /*  240 */, 0x78cb67ecf931fa38ULL /*  241 */,
		0xf33b0372323bbf9cULL /*  242 */, 0x52d66336fb279c74ULL /*  243 */,
		0x505f33ac0afb4eaaULL /*  244 */, 0xe8a5cd99a2cce187ULL /*  245 */,
		0x534974801e2d30bbULL /*  246 */, 0x8d2d5711d5876d90ULL /*  247 */,
		0x1f1a412891bc038eULL /*  248 */, 0xd6e2e71d82e56648ULL /*  249 */,
		0x74036c3a497732b7ULL /*  250 */, 0x89b67ed96361f5abULL /*  251 */,
		0xffed95d8f1ea02a2ULL /*  252 */, 0xe72b3bd61464d43dULL /*  253 */,
		0xa6300f170bdc4820ULL /*  254 */, 0xebc18760ed78a77aULL    /*  255 */
	};
	
	uint64_t sbox2[256] = {
		0xe6a6be5a05a12138ULL /*  256 */, 0xb5a122a5b4f87c98ULL /*  257 */,
		0x563c6089140b6990ULL /*  258 */, 0x4c46cb2e391f5dd5ULL /*  259 */,
		0xd932addbc9b79434ULL /*  260 */, 0x08ea70e42015aff5ULL /*  261 */,
		0xd765a6673e478cf1ULL /*  262 */, 0xc4fb757eab278d99ULL /*  263 */,
		0xdf11c6862d6e0692ULL /*  264 */, 0xddeb84f10d7f3b16ULL /*  265 */,
		0x6f2ef604a665ea04ULL /*  266 */, 0x4a8e0f0ff0e0dfb3ULL /*  267 */,
		0xa5edeef83dbcba51ULL /*  268 */, 0xfc4f0a2a0ea4371eULL /*  269 */,
		0xe83e1da85cb38429ULL /*  270 */, 0xdc8ff882ba1b1ce2ULL /*  271 */,
		0xcd45505e8353e80dULL /*  272 */, 0x18d19a00d4db0717ULL /*  273 */,
		0x34a0cfeda5f38101ULL /*  274 */, 0x0be77e518887caf2ULL /*  275 */,
		0x1e341438b3c45136ULL /*  276 */, 0xe05797f49089ccf9ULL /*  277 */,
		0xffd23f9df2591d14ULL /*  278 */, 0x543dda228595c5cdULL /*  279 */,
		0x661f81fd99052a33ULL /*  280 */, 0x8736e641db0f7b76ULL /*  281 */,
		0x15227725418e5307ULL /*  282 */, 0xe25f7f46162eb2faULL /*  283 */,
		0x48a8b2126c13d9feULL /*  284 */, 0xafdc541792e76eeaULL /*  285 */,
		0x03d912bfc6d1898fULL /*  286 */, 0x31b1aafa1b83f51bULL /*  287 */,
		0xf1ac2796e42ab7d9ULL /*  288 */, 0x40a3a7d7fcd2ebacULL /*  289 */,
		0x1056136d0afbbcc5ULL /*  290 */, 0x7889e1dd9a6d0c85ULL /*  291 */,
		0xd33525782a7974aaULL /*  292 */, 0xa7e25d09078ac09bULL /*  293 */,
		0xbd4138b3eac6edd0ULL /*  294 */, 0x920abfbe71eb9e70ULL /*  295 */,
		0xa2a5d0f54fc2625cULL /*  296 */, 0xc054e36b0b1290a3ULL /*  297 */,
		0xf6dd59ff62fe932bULL /*  298 */, 0x3537354511a8ac7dULL /*  299 */,
		0xca845e9172fadcd4ULL /*  300 */, 0x84f82b60329d20dcULL /*  301 */,
		0x79c62ce1cd672f18ULL /*  302 */, 0x8b09a2add124642cULL /*  303 */,
		0xd0c1e96a19d9e726ULL /*  304 */, 0x5a786a9b4ba9500cULL /*  305 */,
		0x0e020336634c43f3ULL /*  306 */, 0xc17b474aeb66d822ULL /*  307 */,
		0x6a731ae3ec9baac2ULL /*  308 */, 0x8226667ae0840258ULL /*  309 */,
		0x67d4567691caeca5ULL /*  310 */, 0x1d94155c4875adb5ULL /*  311 */,
		0x6d00fd985b813fdfULL /*  312 */, 0x51286efcb774cd06ULL /*  313 */,
		0x5e8834471fa744afULL /*  314 */, 0xf72ca0aee761ae2eULL /*  315 */,
		0xbe40e4cdaee8e09aULL /*  316 */, 0xe9970bbb5118f665ULL /*  317 */,
		0x726e4beb33df1964ULL /*  318 */, 0x703b000729199762ULL /*  319 */,
		0x4631d816f5ef30a7ULL /*  320 */, 0xb880b5b51504a6beULL /*  321 */,
		0x641793c37ed84b6cULL /*  322 */, 0x7b21ed77f6e97d96ULL /*  323 */,
		0x776306312ef96b73ULL /*  324 */, 0xae528948e86ff3f4ULL /*  325 */,
		0x53dbd7f286a3f8f8ULL /*  326 */, 0x16cadce74cfc1063ULL /*  327 */,
		0x005c19bdfa52c6ddULL /*  328 */, 0x68868f5d64d46ad3ULL /*  329 */,
		0x3a9d512ccf1e186aULL /*  330 */, 0x367e62c2385660aeULL /*  331 */,
		0xe359e7ea77dcb1d7ULL /*  332 */, 0x526c0773749abe6eULL /*  333 */,
		0x735ae5f9d09f734bULL /*  334 */, 0x493fc7cc8a558ba8ULL /*  335 */,
		0xb0b9c1533041ab45ULL /*  336 */, 0x321958ba470a59bdULL /*  337 */,
		0x852db00b5f46c393ULL /*  338 */, 0x91209b2bd336b0e5ULL /*  339 */,
		0x6e604f7d659ef19fULL /*  340 */, 0xb99a8ae2782ccb24ULL /*  341 */,
		0xccf52ab6c814c4c7ULL /*  342 */, 0x4727d9afbe11727bULL /*  343 */,
		0x7e950d0c0121b34dULL /*  344 */, 0x756f435670ad471fULL /*  345 */,
		0xf5add442615a6849ULL /*  346 */, 0x4e87e09980b9957aULL /*  347 */,
		0x2acfa1df50aee355ULL /*  348 */, 0xd898263afd2fd556ULL /*  349 */,
		0xc8f4924dd80c8fd6ULL /*  350 */, 0xcf99ca3d754a173aULL /*  351 */,
		0xfe477bacaf91bf3cULL /*  352 */, 0xed5371f6d690c12dULL /*  353 */,
		0x831a5c285e687094ULL /*  354 */, 0xc5d3c90a3708a0a4ULL /*  355 */,
		0x0f7f903717d06580ULL /*  356 */, 0x19f9bb13b8fdf27fULL /*  357 */,
		0xb1bd6f1b4d502843ULL /*  358 */, 0x1c761ba38fff4012ULL /*  359 */,
		0x0d1530c4e2e21f3bULL /*  360 */, 0x8943ce69a7372c8aULL /*  361 */,
		0xe5184e11feb5ce66ULL /*  362 */, 0x618bdb80bd736621ULL /*  363 */,
		0x7d29bad68b574d0bULL /*  364 */, 0x81bb613e25e6fe5bULL /*  365 */,
		0x071c9c10bc07913fULL /*  366 */, 0xc7beeb7909ac2d97ULL /*  367 */,
		0xc3e58d353bc5d757ULL /*  368 */, 0xeb017892f38f61e8ULL /*  369 */,
		0xd4effb9c9b1cc21aULL /*  370 */, 0x99727d26f494f7abULL /*  371 */,
		0xa3e063a2956b3e03ULL /*  372 */, 0x9d4a8b9a4aa09c30ULL /*  373 */,
		0x3f6ab7d500090fb4ULL /*  374 */, 0x9cc0f2a057268ac0ULL /*  375 */,
		0x3dee9d2dedbf42d1ULL /*  376 */, 0x330f49c87960a972ULL /*  377 */,
		0xc6b2720287421b41ULL /*  378 */, 0x0ac59ec07c00369cULL /*  379 */,
		0xef4eac49cb353425ULL /*  380 */, 0xf450244eef0129d8ULL /*  381 */,
		0x8acc46e5caf4deb6ULL /*  382 */, 0x2ffeab63989263f7ULL /*  383 */,
		0x8f7cb9fe5d7a4578ULL /*  384 */, 0x5bd8f7644e634635ULL /*  385 */,
		0x427a7315bf2dc900ULL /*  386 */, 0x17d0c4aa2125261cULL /*  387 */,
		0x3992486c93518e50ULL /*  388 */, 0xb4cbfee0a2d7d4c3ULL /*  389 */,
		0x7c75d6202c5ddd8dULL /*  390 */, 0xdbc295d8e35b6c61ULL /*  391 */,
		0x60b369d302032b19ULL /*  392 */, 0xce42685fdce44132ULL /*  393 */,
		0x06f3ddb9ddf65610ULL /*  394 */, 0x8ea4d21db5e148f0ULL /*  395 */,
		0x20b0fce62fcd496fULL /*  396 */, 0x2c1b912358b0ee31ULL /*  397 */,
		0xb28317b818f5a308ULL /*  398 */, 0xa89c1e189ca6d2cfULL /*  399 */,
		0x0c6b18576aaadbc8ULL /*  400 */, 0xb65deaa91299fae3ULL /*  401 */,
		0xfb2b794b7f1027e7ULL /*  402 */, 0x04e4317f443b5bebULL /*  403 */,
		0x4b852d325939d0a6ULL /*  404 */, 0xd5ae6beefb207ffcULL /*  405 */,
		0x309682b281c7d374ULL /*  406 */, 0xbae309a194c3b475ULL /*  407 */,
		0x8cc3f97b13b49f05ULL /*  408 */, 0x98a9422ff8293967ULL /*  409 */,
		0x244b16b01076ff7cULL /*  410 */, 0xf8bf571c663d67eeULL /*  411 */,
		0x1f0d6758eee30da1ULL /*  412 */, 0xc9b611d97adeb9b7ULL /*  413 */,
		0xb7afd5887b6c57a2ULL /*  414 */, 0x6290ae846b984fe1ULL /*  415 */,
		0x94df4cdeacc1a5fdULL /*  416 */, 0x058a5bd1c5483affULL /*  417 */,
		0x63166cc142ba3c37ULL /*  418 */, 0x8db8526eb2f76f40ULL /*  419 */,
		0xe10880036f0d6d4eULL /*  420 */, 0x9e0523c9971d311dULL /*  421 */,
		0x45ec2824cc7cd691ULL /*  422 */, 0x575b8359e62382c9ULL /*  423 */,
		0xfa9e400dc4889995ULL /*  424 */, 0xd1823ecb45721568ULL /*  425 */,
		0xdafd983b8206082fULL /*  426 */, 0xaa7d29082386a8cbULL /*  427 */,
		0x269fcd4403b87588ULL /*  428 */, 0x1b91f5f728bdd1e0ULL /*  429 */,
		0xe4669f39040201f6ULL /*  430 */, 0x7a1d7c218cf04adeULL /*  431 */,
		0x65623c29d79ce5ceULL /*  432 */, 0x2368449096c00bb1ULL /*  433 */,
		0xab9bf1879da503baULL /*  434 */, 0xbc23ecb1a458058eULL /*  435 */,
		0x9a58df01bb401eccULL /*  436 */, 0xa070e868a85f143dULL /*  437 */,
		0x4ff188307df2239eULL /*  438 */, 0x14d565b41a641183ULL /*  439 */,
		0xee13337452701602ULL /*  440 */, 0x950e3dcf3f285e09ULL /*  441 */,
		0x59930254b9c80953ULL /*  442 */, 0x3bf299408930da6dULL /*  443 */,
		0xa955943f53691387ULL /*  444 */, 0xa15edecaa9cb8784ULL /*  445 */,
		0x29142127352be9a0ULL /*  446 */, 0x76f0371fff4e7afbULL /*  447 */,
		0x0239f450274f2228ULL /*  448 */, 0xbb073af01d5e868bULL /*  449 */,
		0xbfc80571c10e96c1ULL /*  450 */, 0xd267088568222e23ULL /*  451 */,
		0x9671a3d48e80b5b0ULL /*  452 */, 0x55b5d38ae193bb81ULL /*  453 */,
		0x693ae2d0a18b04b8ULL /*  454 */, 0x5c48b4ecadd5335fULL /*  455 */,
		0xfd743b194916a1caULL /*  456 */, 0x2577018134be98c4ULL /*  457 */,
		0xe77987e83c54a4adULL /*  458 */, 0x28e11014da33e1b9ULL /*  459 */,
		0x270cc59e226aa213ULL /*  460 */, 0x71495f756d1a5f60ULL /*  461 */,
		0x9be853fb60afef77ULL /*  462 */, 0xadc786a7f7443dbfULL /*  463 */,
		0x0904456173b29a82ULL /*  464 */, 0x58bc7a66c232bd5eULL /*  465 */,
		0xf306558c673ac8b2ULL /*  466 */, 0x41f639c6b6c9772aULL /*  467 */,
		0x216defe99fda35daULL /*  468 */, 0x11640cc71c7be615ULL /*  469 */,
		0x93c43694565c5527ULL /*  470 */, 0xea038e6246777839ULL /*  471 */,
		0xf9abf3ce5a3e2469ULL /*  472 */, 0x741e768d0fd312d2ULL /*  473 */,
		0x0144b883ced652c6ULL /*  474 */, 0xc20b5a5ba33f8552ULL /*  475 */,
		0x1ae69633c3435a9dULL /*  476 */, 0x97a28ca4088cfdecULL /*  477 */,
		0x8824a43c1e96f420ULL /*  478 */, 0x37612fa66eeea746ULL /*  479 */,
		0x6b4cb165f9cf0e5aULL /*  480 */, 0x43aa1c06a0abfb4aULL /*  481 */,
		0x7f4dc26ff162796bULL /*  482 */, 0x6cbacc8e54ed9b0fULL /*  483 */,
		0xa6b7ffefd2bb253eULL /*  484 */, 0x2e25bc95b0a29d4fULL /*  485 */,
		0x86d6a58bdef1388cULL /*  486 */, 0xded74ac576b6f054ULL /*  487 */,
		0x8030bdbc2b45805dULL /*  488 */, 0x3c81af70e94d9289ULL /*  489 */,
		0x3eff6dda9e3100dbULL /*  490 */, 0xb38dc39fdfcc8847ULL /*  491 */,
		0x123885528d17b87eULL /*  492 */, 0xf2da0ed240b1b642ULL /*  493 */,
		0x44cefadcd54bf9a9ULL /*  494 */, 0x1312200e433c7ee6ULL /*  495 */,
		0x9ffcc84f3a78c748ULL /*  496 */, 0xf0cd1f72248576bbULL /*  497 */,
		0xec6974053638cfe4ULL /*  498 */, 0x2ba7b67c0cec4e4cULL /*  499 */,
		0xac2f4df3e5ce32edULL /*  500 */, 0xcb33d14326ea4c11ULL /*  501 */,
		0xa4e9044cc77e58bcULL /*  502 */, 0x5f513293d934fcefULL /*  503 */,
		0x5dc9645506e55444ULL /*  504 */, 0x50de418f317de40aULL /*  505 */,
		0x388cb31a69dde259ULL /*  506 */, 0x2db4a83455820a86ULL /*  507 */,
		0x9010a91e84711ae9ULL /*  508 */, 0x4df7f0b7b1498371ULL /*  509 */,
		0xd62a2eabc0977179ULL /*  510 */, 0x22fac097aa8d5c0eULL    /*  511 */
	};
	
	uint64_t sbox3[256] = {
		0xf49fcc2ff1daf39bULL /*  512 */, 0x487fd5c66ff29281ULL /*  513 */,
		0xe8a30667fcdca83fULL /*  514 */, 0x2c9b4be3d2fcce63ULL /*  515 */,
		0xda3ff74b93fbbbc2ULL /*  516 */, 0x2fa165d2fe70ba66ULL /*  517 */,
		0xa103e279970e93d4ULL /*  518 */, 0xbecdec77b0e45e71ULL /*  519 */,
		0xcfb41e723985e497ULL /*  520 */, 0xb70aaa025ef75017ULL /*  521 */,
		0xd42309f03840b8e0ULL /*  522 */, 0x8efc1ad035898579ULL /*  523 */,
		0x96c6920be2b2abc5ULL /*  524 */, 0x66af4163375a9172ULL /*  525 */,
		0x2174abdcca7127fbULL /*  526 */, 0xb33ccea64a72ff41ULL /*  527 */,
		0xf04a4933083066a5ULL /*  528 */, 0x8d970acdd7289af5ULL /*  529 */,
		0x8f96e8e031c8c25eULL /*  530 */, 0xf3fec02276875d47ULL /*  531 */,
		0xec7bf310056190ddULL /*  532 */, 0xf5adb0aebb0f1491ULL /*  533 */,
		0x9b50f8850fd58892ULL /*  534 */, 0x4975488358b74de8ULL /*  535 */,
		0xa3354ff691531c61ULL /*  536 */, 0x0702bbe481d2c6eeULL /*  537 */,
		0x89fb24057deded98ULL /*  538 */, 0xac3075138596e902ULL /*  539 */,
		0x1d2d3580172772edULL /*  540 */, 0xeb738fc28e6bc30dULL /*  541 */,
		0x5854ef8f63044326ULL /*  542 */, 0x9e5c52325add3bbeULL /*  543 */,
		0x90aa53cf325c4623ULL /*  544 */, 0xc1d24d51349dd067ULL /*  545 */,
		0x2051cfeea69ea624ULL /*  546 */, 0x13220f0a862e7e4fULL /*  547 */,
		0xce39399404e04864ULL /*  548 */, 0xd9c42ca47086fcb7ULL /*  549 */,
		0x685ad2238a03e7ccULL /*  550 */, 0x066484b2ab2ff1dbULL /*  551 */,
		0xfe9d5d70efbf79ecULL /*  552 */, 0x5b13b9dd9c481854ULL /*  553 */,
		0x15f0d475ed1509adULL /*  554 */, 0x0bebcd060ec79851ULL /*  555 */,
		0xd58c6791183ab7f8ULL /*  556 */, 0xd1187c5052f3eee4ULL /*  557 */,
		0xc95d1192e54e82ffULL /*  558 */, 0x86eea14cb9ac6ca2ULL /*  559 */,
		0x3485beb153677d5dULL /*  560 */, 0xdd191d781f8c492aULL /*  561 */,
		0xf60866baa784ebf9ULL /*  562 */, 0x518f643ba2d08c74ULL /*  563 */,
		0x8852e956e1087c22ULL /*  564 */, 0xa768cb8dc410ae8dULL /*  565 */,
		0x38047726bfec8e1aULL /*  566 */, 0xa67738b4cd3b45aaULL /*  567 */,
		0xad16691cec0dde19ULL /*  568 */, 0xc6d4319380462e07ULL /*  569 */,
		0xc5a5876d0ba61938ULL /*  570 */, 0x16b9fa1fa58fd840ULL /*  571 */,
		0x188ab1173ca74f18ULL /*  572 */, 0xabda2f98c99c021fULL /*  573 */,
		0x3e0580ab134ae816ULL /*  574 */, 0x5f3b05b773645abbULL /*  575 */,
		0x2501a2be5575f2f6ULL /*  576 */, 0x1b2f74004e7e8ba9ULL /*  577 */,
		0x1cd7580371e8d953ULL /*  578 */, 0x7f6ed89562764e30ULL /*  579 */,
		0xb15926ff596f003dULL /*  580 */, 0x9f65293da8c5d6b9ULL /*  581 */,
		0x6ecef04dd690f84cULL /*  582 */, 0x4782275fff33af88ULL /*  583 */,
		0xe41433083f820801ULL /*  584 */, 0xfd0dfe409a1af9b5ULL /*  585 */,
		0x4325a3342cdb396bULL /*  586 */, 0x8ae77e62b301b252ULL /*  587 */,
		0xc36f9e9f6655615aULL /*  588 */, 0x85455a2d92d32c09ULL /*  589 */,
		0xf2c7dea949477485ULL /*  590 */, 0x63cfb4c133a39ebaULL /*  591 */,
		0x83b040cc6ebc5462ULL /*  592 */, 0x3b9454c8fdb326b0ULL /*  593 */,
		0x56f56a9e87ffd78cULL /*  594 */, 0x2dc2940d99f42bc6ULL /*  595 */,
		0x98f7df096b096e2dULL /*  596 */, 0x19a6e01e3ad852bfULL /*  597 */,
		0x42a99ccbdbd4b40bULL /*  598 */, 0xa59998af45e9c559ULL /*  599 */,
		0x366295e807d93186ULL /*  600 */, 0x6b48181bfaa1f773ULL /*  601 */,
		0x1fec57e2157a0a1dULL /*  602 */, 0x4667446af6201ad5ULL /*  603 */,
		0xe615ebcacfb0f075ULL /*  604 */, 0xb8f31f4f68290778ULL /*  605 */,
		0x22713ed6ce22d11eULL /*  606 */, 0x3057c1a72ec3c93bULL /*  607 */,
		0xcb46acc37c3f1f2fULL /*  608 */, 0xdbb893fd02aaf50eULL /*  609 */,
		0x331fd92e600b9fcfULL /*  610 */, 0xa498f96148ea3ad6ULL /*  611 */,
		0xa8d8426e8b6a83eaULL /*  612 */, 0xa089b274b7735cdcULL /*  613 */,
		0x87f6b3731e524a11ULL /*  614 */, 0x118808e5cbc96749ULL /*  615 */,
		0x9906e4c7b19bd394ULL /*  616 */, 0xafed7f7e9b24a20cULL /*  617 */,
		0x6509eadeeb3644a7ULL /*  618 */, 0x6c1ef1d3e8ef0edeULL /*  619 */,
		0xb9c97d43e9798fb4ULL /*  620 */, 0xa2f2d784740c28a3ULL /*  621 */,
		0x7b8496476197566fULL /*  622 */, 0x7a5be3e6b65f069dULL /*  623 */,
		0xf96330ed78be6f10ULL /*  624 */, 0xeee60de77a076a15ULL /*  625 */,
		0x2b4bee4aa08b9bd0ULL /*  626 */, 0x6a56a63ec7b8894eULL /*  627 */,
		0x02121359ba34fef4ULL /*  628 */, 0x4cbf99f8283703fcULL /*  629 */,
		0x398071350caf30c8ULL /*  630 */, 0xd0a77a89f017687aULL /*  631 */,
		0xf1c1a9eb9e423569ULL /*  632 */, 0x8c7976282dee8199ULL /*  633 */,
		0x5d1737a5dd1f7abdULL /*  634 */, 0x4f53433c09a9fa80ULL /*  635 */,
		0xfa8b0c53df7ca1d9ULL /*  636 */, 0x3fd9dcbc886ccb77ULL /*  637 */,
		0xc040917ca91b4720ULL /*  638 */, 0x7dd00142f9d1dcdfULL /*  639 */,
		0x8476fc1d4f387b58ULL /*  640 */, 0x23f8e7c5f3316503ULL /*  641 */,
		0x032a2244e7e37339ULL /*  642 */, 0x5c87a5d750f5a74bULL /*  643 */,
		0x082b4cc43698992eULL /*  644 */, 0xdf917becb858f63cULL /*  645 */,
		0x3270b8fc5bf86ddaULL /*  646 */, 0x10ae72bb29b5dd76ULL /*  647 */,
		0x576ac94e7700362bULL /*  648 */, 0x1ad112dac61efb8fULL /*  649 */,
		0x691bc30ec5faa427ULL /*  650 */, 0xff246311cc327143ULL /*  651 */,
		0x3142368e30e53206ULL /*  652 */, 0x71380e31e02ca396ULL /*  653 */,
		0x958d5c960aad76f1ULL /*  654 */, 0xf8d6f430c16da536ULL /*  655 */,
		0xc8ffd13f1be7e1d2ULL /*  656 */, 0x7578ae66004ddbe1ULL /*  657 */,
		0x05833f01067be646ULL /*  658 */, 0xbb34b5ad3bfe586dULL /*  659 */,
		0x095f34c9a12b97f0ULL /*  660 */, 0x247ab64525d60ca8ULL /*  661 */,
		0xdcdbc6f3017477d1ULL /*  662 */, 0x4a2e14d4decad24dULL /*  663 */,
		0xbdb5e6d9be0a1eebULL /*  664 */, 0x2a7e70f7794301abULL /*  665 */,
		0xdef42d8a270540fdULL /*  666 */, 0x01078ec0a34c22c1ULL /*  667 */,
		0xe5de511af4c16387ULL /*  668 */, 0x7ebb3a52bd9a330aULL /*  669 */,
		0x77697857aa7d6435ULL /*  670 */, 0x004e831603ae4c32ULL /*  671 */,
		0xe7a21020ad78e312ULL /*  672 */, 0x9d41a70c6ab420f2ULL /*  673 */,
		0x28e06c18ea1141e6ULL /*  674 */, 0xd2b28cbd984f6b28ULL /*  675 */,
		0x26b75f6c446e9d83ULL /*  676 */, 0xba47568c4d418d7fULL /*  677 */,
		0xd80badbfe6183d8eULL /*  678 */, 0x0e206d7f5f166044ULL /*  679 */,
		0xe258a43911cbca3eULL /*  680 */, 0x723a1746b21dc0bcULL /*  681 */,
		0xc7caa854f5d7cdd3ULL /*  682 */, 0x7cac32883d261d9cULL /*  683 */,
		0x7690c26423ba942cULL /*  684 */, 0x17e55524478042b8ULL /*  685 */,
		0xe0be477656a2389fULL /*  686 */, 0x4d289b5e67ab2da0ULL /*  687 */,
		0x44862b9c8fbbfd31ULL /*  688 */, 0xb47cc8049d141365ULL /*  689 */,
		0x822c1b362b91c793ULL /*  690 */, 0x4eb14655fb13dfd8ULL /*  691 */,
		0x1ecbba0714e2a97bULL /*  692 */, 0x6143459d5cde5f14ULL /*  693 */,
		0x53a8fbf1d5f0ac89ULL /*  694 */, 0x97ea04d81c5e5b00ULL /*  695 */,
		0x622181a8d4fdb3f3ULL /*  696 */, 0xe9bcd341572a1208ULL /*  697 */,
		0x1411258643cce58aULL /*  698 */, 0x9144c5fea4c6e0a4ULL /*  699 */,
		0x0d33d06565cf620fULL /*  700 */, 0x54a48d489f219ca1ULL /*  701 */,
		0xc43e5eac6d63c821ULL /*  702 */, 0xa9728b3a72770dafULL /*  703 */,
		0xd7934e7b20df87efULL /*  704 */, 0xe35503b61a3e86e5ULL /*  705 */,
		0xcae321fbc819d504ULL /*  706 */, 0x129a50b3ac60bfa6ULL /*  707 */,
		0xcd5e68ea7e9fb6c3ULL /*  708 */, 0xb01c90199483b1c7ULL /*  709 */,
		0x3de93cd5c295376cULL /*  710 */, 0xaed52edf2ab9ad13ULL /*  711 */,
		0x2e60f512c0a07884ULL /*  712 */, 0xbc3d86a3e36210c9ULL /*  713 */,
		0x35269d9b163951ceULL /*  714 */, 0x0c7d6e2ad0cdb5faULL /*  715 */,
		0x59e86297d87f5733ULL /*  716 */, 0x298ef221898db0e7ULL /*  717 */,
		0x55000029d1a5aa7eULL /*  718 */, 0x8bc08ae1b5061b45ULL /*  719 */,
		0xc2c31c2b6c92703aULL /*  720 */, 0x94cc596baf25ef42ULL /*  721 */,
		0x0a1d73db22540456ULL /*  722 */, 0x04b6a0f9d9c4179aULL /*  723 */,
		0xeffdafa2ae3d3c60ULL /*  724 */, 0xf7c8075bb49496c4ULL /*  725 */,
		0x9cc5c7141d1cd4e3ULL /*  726 */, 0x78bd1638218e5534ULL /*  727 */,
		0xb2f11568f850246aULL /*  728 */, 0xedfabcfa9502bc29ULL /*  729 */,
		0x796ce5f2da23051bULL /*  730 */, 0xaae128b0dc93537cULL /*  731 */,
		0x3a493da0ee4b29aeULL /*  732 */, 0xb5df6b2c416895d7ULL /*  733 */,
		0xfcabbd25122d7f37ULL /*  734 */, 0x70810b58105dc4b1ULL /*  735 */,
		0xe10fdd37f7882a90ULL /*  736 */, 0x524dcab5518a3f5cULL /*  737 */,
		0x3c9e85878451255bULL /*  738 */, 0x4029828119bd34e2ULL /*  739 */,
		0x74a05b6f5d3ceccbULL /*  740 */, 0xb610021542e13ecaULL /*  741 */,
		0x0ff979d12f59e2acULL /*  742 */, 0x6037da27e4f9cc50ULL /*  743 */,
		0x5e92975a0df1847dULL /*  744 */, 0xd66de190d3e623feULL /*  745 */,
		0x5032d6b87b568048ULL /*  746 */, 0x9a36b7ce8235216eULL /*  747 */,
		0x80272a7a24f64b4aULL /*  748 */, 0x93efed8b8c6916f7ULL /*  749 */,
		0x37ddbff44cce1555ULL /*  750 */, 0x4b95db5d4b99bd25ULL /*  751 */,
		0x92d3fda169812fc0ULL /*  752 */, 0xfb1a4a9a90660bb6ULL /*  753 */,
		0x730c196946a4b9b2ULL /*  754 */, 0x81e289aa7f49da68ULL /*  755 */,
		0x64669a0f83b1a05fULL /*  756 */, 0x27b3ff7d9644f48bULL /*  757 */,
		0xcc6b615c8db675b3ULL /*  758 */, 0x674f20b9bcebbe95ULL /*  759 */,
		0x6f31238275655982ULL /*  760 */, 0x5ae488713e45cf05ULL /*  761 */,
		0xbf619f9954c21157ULL /*  762 */, 0xeabac46040a8eae9ULL /*  763 */,
		0x454c6fe9f2c0c1cdULL /*  764 */, 0x419cf6496412691cULL /*  765 */,
		0xd3dc3bef265b0f70ULL /*  766 */, 0x6d0e60f5c3578a9eULL    /*  767 */
	};
	
	uint64_t sbox4[256] = {
		0x5b0e608526323c55ULL /*  768 */, 0x1a46c1a9fa1b59f5ULL /*  769 */,
		0xa9e245a17c4c8ffaULL /*  770 */, 0x65ca5159db2955d7ULL /*  771 */,
		0x05db0a76ce35afc2ULL /*  772 */, 0x81eac77ea9113d45ULL /*  773 */,
		0x528ef88ab6ac0a0dULL /*  774 */, 0xa09ea253597be3ffULL /*  775 */,
		0x430ddfb3ac48cd56ULL /*  776 */, 0xc4b3a67af45ce46fULL /*  777 */,
		0x4ececfd8fbe2d05eULL /*  778 */, 0x3ef56f10b39935f0ULL /*  779 */,
		0x0b22d6829cd619c6ULL /*  780 */, 0x17fd460a74df2069ULL /*  781 */,
		0x6cf8cc8e8510ed40ULL /*  782 */, 0xd6c824bf3a6ecaa7ULL /*  783 */,
		0x61243d581a817049ULL /*  784 */, 0x048bacb6bbc163a2ULL /*  785 */,
		0xd9a38ac27d44cc32ULL /*  786 */, 0x7fddff5baaf410abULL /*  787 */,
		0xad6d495aa804824bULL /*  788 */, 0xe1a6a74f2d8c9f94ULL /*  789 */,
		0xd4f7851235dee8e3ULL /*  790 */, 0xfd4b7f886540d893ULL /*  791 */,
		0x247c20042aa4bfdaULL /*  792 */, 0x096ea1c517d1327cULL /*  793 */,
		0xd56966b4361a6685ULL /*  794 */, 0x277da5c31221057dULL /*  795 */,
		0x94d59893a43acff7ULL /*  796 */, 0x64f0c51ccdc02281ULL /*  797 */,
		0x3d33bcc4ff6189dbULL /*  798 */, 0xe005cb184ce66af1ULL /*  799 */,
		0xff5ccd1d1db99beaULL /*  800 */, 0xb0b854a7fe42980fULL /*  801 */,
		0x7bd46a6a718d4b9fULL /*  802 */, 0xd10fa8cc22a5fd8cULL /*  803 */,
		0xd31484952be4bd31ULL /*  804 */, 0xc7fa975fcb243847ULL /*  805 */,
		0x4886ed1e5846c407ULL /*  806 */, 0x28cddb791eb70b04ULL /*  807 */,
		0xc2b00be2f573417fULL /*  808 */, 0x5c9590452180f877ULL /*  809 */,
		0x7a6bddfff370eb00ULL /*  810 */, 0xce509e38d6d9d6a4ULL /*  811 */,
		0xebeb0f00647fa702ULL /*  812 */, 0x1dcc06cf76606f06ULL /*  813 */,
		0xe4d9f28ba286ff0aULL /*  814 */, 0xd85a305dc918c262ULL /*  815 */,
		0x475b1d8732225f54ULL /*  816 */, 0x2d4fb51668ccb5feULL /*  817 */,
		0xa679b9d9d72bba20ULL /*  818 */, 0x53841c0d912d43a5ULL /*  819 */,
		0x3b7eaa48bf12a4e8ULL /*  820 */, 0x781e0e47f22f1ddfULL /*  821 */,
		0xeff20ce60ab50973ULL /*  822 */, 0x20d261d19dffb742ULL /*  823 */,
		0x16a12b03062a2e39ULL /*  824 */, 0x1960eb2239650495ULL /*  825 */,
		0x251c16fed50eb8b8ULL /*  826 */, 0x9ac0c330f826016eULL /*  827 */,
		0xed152665953e7671ULL /*  828 */, 0x02d63194a6369570ULL /*  829 */,
		0x5074f08394b1c987ULL /*  830 */, 0x70ba598c90b25ce1ULL /*  831 */,
		0x794a15810b9742f6ULL /*  832 */, 0x0d5925e9fcaf8c6cULL /*  833 */,
		0x3067716cd868744eULL /*  834 */, 0x910ab077e8d7731bULL /*  835 */,
		0x6a61bbdb5ac42f61ULL /*  836 */, 0x93513efbf0851567ULL /*  837 */,
		0xf494724b9e83e9d5ULL /*  838 */, 0xe887e1985c09648dULL /*  839 */,
		0x34b1d3c675370cfdULL /*  840 */, 0xdc35e433bc0d255dULL /*  841 */,
		0xd0aab84234131be0ULL /*  842 */, 0x08042a50b48b7eafULL /*  843 */,
		0x9997c4ee44a3ab35ULL /*  844 */, 0x829a7b49201799d0ULL /*  845 */,
		0x263b8307b7c54441ULL /*  846 */, 0x752f95f4fd6a6ca6ULL /*  847 */,
		0x927217402c08c6e5ULL /*  848 */, 0x2a8ab754a795d9eeULL /*  849 */,
		0xa442f7552f72943dULL /*  850 */, 0x2c31334e19781208ULL /*  851 */,
		0x4fa98d7ceaee6291ULL /*  852 */, 0x55c3862f665db309ULL /*  853 */,
		0xbd0610175d53b1f3ULL /*  854 */, 0x46fe6cb840413f27ULL /*  855 */,
		0x3fe03792df0cfa59ULL /*  856 */, 0xcfe700372eb85e8fULL /*  857 */,
		0xa7be29e7adbce118ULL /*  858 */, 0xe544ee5cde8431ddULL /*  859 */,
		0x8a781b1b41f1873eULL /*  860 */, 0xa5c94c78a0d2f0e7ULL /*  861 */,
		0x39412e2877b60728ULL /*  862 */, 0xa1265ef3afc9a62cULL /*  863 */,
		0xbcc2770c6a2506c5ULL /*  864 */, 0x3ab66dd5dce1ce12ULL /*  865 */,
		0xe65499d04a675b37ULL /*  866 */, 0x7d8f523481bfd216ULL /*  867 */,
		0x0f6f64fcec15f389ULL /*  868 */, 0x74efbe618b5b13c8ULL /*  869 */,
		0xacdc82b714273e1dULL /*  870 */, 0xdd40bfe003199d17ULL /*  871 */,
		0x37e99257e7e061f8ULL /*  872 */, 0xfa52626904775aaaULL /*  873 */,
		0x8bbbf63a463d56f9ULL /*  874 */, 0xf0013f1543a26e64ULL /*  875 */,
		0xa8307e9f879ec898ULL /*  876 */, 0xcc4c27a4150177ccULL /*  877 */,
		0x1b432f2cca1d3348ULL /*  878 */, 0xde1d1f8f9f6fa013ULL /*  879 */,
		0x606602a047a7ddd6ULL /*  880 */, 0xd237ab64cc1cb2c7ULL /*  881 */,
		0x9b938e7225fcd1d3ULL /*  882 */, 0xec4e03708e0ff476ULL /*  883 */,
		0xfeb2fbda3d03c12dULL /*  884 */, 0xae0bced2ee43889aULL /*  885 */,
		0x22cb8923ebfb4f43ULL /*  886 */, 0x69360d013cf7396dULL /*  887 */,
		0x855e3602d2d4e022ULL /*  888 */, 0x073805bad01f784cULL /*  889 */,
		0x33e17a133852f546ULL /*  890 */, 0xdf4874058ac7b638ULL /*  891 */,
		0xba92b29c678aa14aULL /*  892 */, 0x0ce89fc76cfaadcdULL /*  893 */,
		0x5f9d4e0908339e34ULL /*  894 */, 0xf1afe9291f5923b9ULL /*  895 */,
		0x6e3480f60f4a265fULL /*  896 */, 0xeebf3a2ab29b841cULL /*  897 */,
		0xe21938a88f91b4adULL /*  898 */, 0x57dfeff845c6d3c3ULL /*  899 */,
		0x2f006b0bf62caaf2ULL /*  900 */, 0x62f479ef6f75ee78ULL /*  901 */,
		0x11a55ad41c8916a9ULL /*  902 */, 0xf229d29084fed453ULL /*  903 */,
		0x42f1c27b16b000e6ULL /*  904 */, 0x2b1f76749823c074ULL /*  905 */,
		0x4b76eca3c2745360ULL /*  906 */, 0x8c98f463b91691bdULL /*  907 */,
		0x14bcc93cf1ade66aULL /*  908 */, 0x8885213e6d458397ULL /*  909 */,
		0x8e177df0274d4711ULL /*  910 */, 0xb49b73b5503f2951ULL /*  911 */,
		0x10168168c3f96b6bULL /*  912 */, 0x0e3d963b63cab0aeULL /*  913 */,
		0x8dfc4b5655a1db14ULL /*  914 */, 0xf789f1356e14de5cULL /*  915 */,
		0x683e68af4e51dac1ULL /*  916 */, 0xc9a84f9d8d4b0fd9ULL /*  917 */,
		0x3691e03f52a0f9d1ULL /*  918 */, 0x5ed86e46e1878e80ULL /*  919 */,
		0x3c711a0e99d07150ULL /*  920 */, 0x5a0865b20c4e9310ULL /*  921 */,
		0x56fbfc1fe4f0682eULL /*  922 */, 0xea8d5de3105edf9bULL /*  923 */,
		0x71abfdb12379187aULL /*  924 */, 0x2eb99de1bee77b9cULL /*  925 */,
		0x21ecc0ea33cf4523ULL /*  926 */, 0x59a4d7521805c7a1ULL /*  927 */,
		0x3896f5eb56ae7c72ULL /*  928 */, 0xaa638f3db18f75dcULL /*  929 */,
		0x9f39358dabe9808eULL /*  930 */, 0xb7defa91c00b72acULL /*  931 */,
		0x6b5541fd62492d92ULL /*  932 */, 0x6dc6dee8f92e4d5bULL /*  933 */,
		0x353f57abc4beea7eULL /*  934 */, 0x735769d6da5690ceULL /*  935 */,
		0x0a234aa642391484ULL /*  936 */, 0xf6f9508028f80d9dULL /*  937 */,
		0xb8e319a27ab3f215ULL /*  938 */, 0x31ad9c1151341a4dULL /*  939 */,
		0x773c22a57bef5805ULL /*  940 */, 0x45c7561a07968633ULL /*  941 */,
		0xf913da9e249dbe36ULL /*  942 */, 0xda652d9b78a64c68ULL /*  943 */,
		0x4c27a97f3bc334efULL /*  944 */, 0x76621220e66b17f4ULL /*  945 */,
		0x967743899acd7d0bULL /*  946 */, 0xf3ee5bcae0ed6782ULL /*  947 */,
		0x409f753600c879fcULL /*  948 */, 0x06d09a39b5926db6ULL /*  949 */,
		0x6f83aeb0317ac588ULL /*  950 */, 0x01e6ca4a86381f21ULL /*  951 */,
		0x66ff3462d19f3025ULL /*  952 */, 0x72207c24ddfd3bfbULL /*  953 */,
		0x4af6b6d3e2ece2ebULL /*  954 */, 0x9c994dbec7ea08deULL /*  955 */,
		0x49ace597b09a8bc4ULL /*  956 */, 0xb38c4766cf0797baULL /*  957 */,
		0x131b9373c57c2a75ULL /*  958 */, 0xb1822cce61931e58ULL /*  959 */,
		0x9d7555b909ba1c0cULL /*  960 */, 0x127fafdd937d11d2ULL /*  961 */,
		0x29da3badc66d92e4ULL /*  962 */, 0xa2c1d57154c2ecbcULL /*  963 */,
		0x58c5134d82f6fe24ULL /*  964 */, 0x1c3ae3515b62274fULL /*  965 */,
		0xe907c82e01cb8126ULL /*  966 */, 0xf8ed091913e37fcbULL /*  967 */,
		0x3249d8f9c80046c9ULL /*  968 */, 0x80cf9bede388fb63ULL /*  969 */,
		0x1881539a116cf19eULL /*  970 */, 0x5103f3f76bd52457ULL /*  971 */,
		0x15b7e6f5ae47f7a8ULL /*  972 */, 0xdbd7c6ded47e9ccfULL /*  973 */,
		0x44e55c410228bb1aULL /*  974 */, 0xb647d4255edb4e99ULL /*  975 */,
		0x5d11882bb8aafc30ULL /*  976 */, 0xf5098bbb29d3212aULL /*  977 */,
		0x8fb5ea14e90296b3ULL /*  978 */, 0x677b942157dd025aULL /*  979 */,
		0xfb58e7c0a390acb5ULL /*  980 */, 0x89d3674c83bd4a01ULL /*  981 */,
		0x9e2da4df4bf3b93bULL /*  982 */, 0xfcc41e328cab4829ULL /*  983 */,
		0x03f38c96ba582c52ULL /*  984 */, 0xcad1bdbd7fd85db2ULL /*  985 */,
		0xbbb442c16082ae83ULL /*  986 */, 0xb95fe86ba5da9ab0ULL /*  987 */,
		0xb22e04673771a93fULL /*  988 */, 0x845358c9493152d8ULL /*  989 */,
		0xbe2a488697b4541eULL /*  990 */, 0x95a2dc2dd38e6966ULL /*  991 */,
		0xc02c11ac923c852bULL /*  992 */, 0x2388b1990df2a87bULL /*  993 */,
		0x7c8008fa1b4f37beULL /*  994 */, 0x1f70d0c84d54e503ULL /*  995 */,
		0x5490adec7ece57d4ULL /*  996 */, 0x002b3c27d9063a3aULL /*  997 */,
		0x7eaea3848030a2bfULL /*  998 */, 0xc602326ded2003c0ULL /*  999 */,
		0x83a7287d69a94086ULL /* 1000 */, 0xc57a5fcb30f57a8aULL /* 1001 */,
		0xb56844e479ebe779ULL /* 1002 */, 0xa373b40f05dcbce9ULL /* 1003 */,
		0xd71a786e88570ee2ULL /* 1004 */, 0x879cbacdbde8f6a0ULL /* 1005 */,
		0x976ad1bcc164a32fULL /* 1006 */, 0xab21e25e9666d78bULL /* 1007 */,
		0x901063aae5e5c33cULL /* 1008 */, 0x9818b34448698d90ULL /* 1009 */,
		0xe36487ae3e1e8abbULL /* 1010 */, 0xafbdf931893bdcb4ULL /* 1011 */,
		0x6345a0dc5fbbd519ULL /* 1012 */, 0x8628fe269b9465caULL /* 1013 */,
		0x1e5d01603f9c51ecULL /* 1014 */, 0x4de44006a15049b7ULL /* 1015 */,
		0xbf6c70e5f776cbb1ULL /* 1016 */, 0x411218f2ef552bedULL /* 1017 */,
		0xcb0c0708705a36a3ULL /* 1018 */, 0xe74d14754f986044ULL /* 1019 */,
		0xcd56d9430ea8280eULL /* 1020 */, 0xc12591d7535f5065ULL /* 1021 */,
		0xc83223f1720aef96ULL /* 1022 */, 0xc3a0396f7363a51fULL    /* 1023 */
	};
	
	
#if TARGET_CPU_BITS == 64
	/* This is the official definition of round */
#define tiger_round(a, b, c, x, mul) \
			c ^= x; \
			a -= sbox1[((c) >> (0 * 8)) & 0xFF] ^ sbox2[((c) >> (2 * 8)) & 0xFF] ^ \
				 sbox3[((c) >> (4 * 8)) & 0xFF] ^ sbox4[((c) >> (6 * 8)) & 0xFF] ; \
			b += sbox4[((c) >> (1 * 8)) & 0xFF] ^ sbox3[((c) >> (3 * 8)) & 0xFF] ^ \
				 sbox2[((c) >> (5 * 8)) & 0xFF] ^ sbox1[((c) >> (7 * 8)) & 0xFF] ; \
			b *= mul;
#else
	/* This code works faster when compiled on 32-bit machines */
	/* (but works slower on Alpha) */
#define tiger_round(a, b, c, x, mul) \
			c ^= x; \
			a -= sbox1[(uint8_t)(c)] ^ \
				 sbox2[(uint8_t)(((uint32_t)(c)) >> (2 * 8))] ^ \
				 sbox3[(uint8_t)((c) >> (4 * 8))] ^ \
				 sbox4[(uint8_t)(((uint32_t)((c) >> (4 * 8))) >> (2 * 8))] ; \
			b += sbox4[(uint8_t)(((uint32_t)(c)) >> (1 * 8))] ^ \
				 sbox3[(uint8_t)(((uint32_t)(c)) >> (3 * 8))] ^ \
				 sbox2[(uint8_t)(((uint32_t)((c) >> (4 * 8))) >> (1 * 8))] ^ \
				 sbox1[(uint8_t)(((uint32_t)((c) >> (4 * 8))) >> (3 * 8))]; \
			b *= mul;
#endif
	
#define pass(a, b, c, mul) \
			tiger_round(a, b, c, x[0], mul) \
			tiger_round(b, c, a, x[1], mul) \
			tiger_round(c, a, b, x[2], mul) \
			tiger_round(a, b, c, x[3], mul) \
			tiger_round(b, c, a, x[4], mul) \
			tiger_round(c, a, b, x[5], mul) \
			tiger_round(a, b, c, x[6], mul) \
			tiger_round(b, c, a, x[7], mul)
	
#define key_schedule \
			x[0] -= x[7] ^ 0xA5A5A5A5A5A5A5A5LL; \
			x[1] ^= x[0]; \
			x[2] += x[1]; \
			x[3] -= x[2] ^ ((~x[1]) << 19); \
			x[4] ^= x[3]; \
			x[5] += x[4]; \
			x[6] -= x[5] ^ ((~x[4]) >> 23); \
			x[7] ^= x[6]; \
			x[0] += x[7]; \
			x[1] -= x[0] ^ ((~x[7]) << 19); \
			x[2] ^= x[1]; \
			x[3] += x[2]; \
			x[4] -= x[3] ^ ((~x[2]) >> 23); \
			x[5] ^= x[4]; \
			x[6] += x[5]; \
			x[7] -= x[6] ^ 0x0123456789ABCDEFLL;
	
#if TARGET_CPU_BITS == 64
	/* The loop is unrolled: works better on Alpha */
#define compress \
			save_abc \
			pass(a, b, c, 5) \
			key_schedule \
			pass(c, a, b, 7) \
			key_schedule \
			pass(b, c, a, 9) \
			for (pass_no = 3; pass_no < PASSES; pass_no++) { \
			key_schedule \
			pass(a, b, c, 9) \
			tmpa = a; a = c; c = b; b = tmpa;} \
			feedforward
#else
	/* loop: works better on PC and Sun (smaller cache?) */
#define compress \
			save_abc \
			for (pass_no = 0; pass_no < PASSES; pass_no++) { \
			if (pass_no != 0) {key_schedule} \
			pass(a, b, c, (pass_no == 0 ? 5 : pass_no == 1 ? 7 : 9)); \
			tmpa = a; a = c; c = b; b = tmpa;} \
			feedforward
#endif
	
#define tiger_compress_macro(str, state) \
		{ \
			word64 a, b, c, tmpa; \
			word64 aa, bb, cc; \
			word64 x0, x1, x2, x3, x4, x5, x6, x7; \
			int pass_no; \
	\
			a = state[0]; \
			b = state[1]; \
			c = state[2]; \
	\
			x0 = str[0]; x1 = str[1]; x2 = str[2]; x3 = str[3]; \
			x4 = str[4]; x5 = str[5]; x6 = str[6]; x7 = str[7]; \
	\
			compress; \
	\
			state[0] = a; \
			state[1] = b; \
			state[2] = c; \
		}
	
	
#define save_abc \
			aa = a; \
			bb = b; \
			cc = c;
	
#define feedforward \
			a ^= aa; \
			b -= bb; \
			c += cc;
	
#define tiger_compress(str, state) \
			tiger_compress_macro(((word64 *)str), ((word64 *)state))
	
	/****************
	 * Transform the message DATA which consists of 512 bytes (8 words)
	 */
	void transform(cc_tiger_ctx *hd, unsigned char *data)
	{
		uint64_t a, b, c, aa, bb, cc;
		uint64_t x[8];
	  #ifdef TARGET_BIG_ENDIAN
	#define MKWORD(d, n) \
		(((uint64_t)(d)[8 * (n) + 7]) << 56 | ((uint64_t)(d)[8 * (n) + 6]) << 48  \
		 | ((uint64_t)(d)[8 * (n) + 5]) << 40 | ((uint64_t)(d)[8 * (n) + 4]) << 32  \
		 | ((uint64_t)(d)[8 * (n) + 3]) << 24 | ((uint64_t)(d)[8 * (n) + 2]) << 16  \
		 | ((uint64_t)(d)[8 * (n) + 1]) << 8 | ((uint64_t)(d)[8 * (n)        ]))
		x[0] = MKWORD(data, 0);
		x[1] = MKWORD(data, 1);
		x[2] = MKWORD(data, 2);
		x[3] = MKWORD(data, 3);
		x[4] = MKWORD(data, 4);
		x[5] = MKWORD(data, 5);
		x[6] = MKWORD(data, 6);
		x[7] = MKWORD(data, 7);
	#undef MKWORD
	  #else
		memcpy(x, data, 64);
	  #endif
	
		/* save */
		a = aa = hd->a;
		b = bb = hd->b;
		c = cc = hd->c;
	
		/* print functions have been removed (jk) */
		pass(a, b, c, 5);
		key_schedule;
		pass(c, a, b, 7);
		key_schedule;
		pass(b, c, a, 9);
	
		/* feedforward */
		a ^= aa;
		b -= bb;
		c += cc;
		/* store */
		hd->a = a;
		hd->b = b;
		hd->c = c;
	}
	
	void tiger_init(cc_tiger_ctx *hd)
	{
		hd->a = 0x0123456789abcdefLL;
		hd->b = 0xfedcba9876543210LL;
		hd->c = 0xf096a5b4c3b2e187LL;
		hd->nblocks = 0;
		hd->count = 0;
	}
	
	/* Update the message digest with the contents
	 * of INBUF with length INLEN. */
	void tiger_update(cc_tiger_ctx *hd, unsigned char *inbuf, size_t inlen)
	{
		if (hd->count == 64) {  /* flush the buffer */
			transform(hd, hd->buf);
			hd->count = 0;
			hd->nblocks++;
		}
	
		if (!inbuf)
			return;
	
		if (hd->count) {
			for ( ; inlen && hd->count < 64; inlen--)
				hd->buf[hd->count++] = *inbuf++;
	
			if (hd->count == 64) {          /* flush the buffer */
				transform(hd, hd->buf);
				hd->count = 0;
				hd->nblocks++;
			}
	
			if (!inlen) {
				return;
			}
		}
	
		while (inlen >= 64) {
			transform(hd, inbuf);
			hd->count = 0;
			hd->nblocks++;
			inlen -= 64;
			inbuf += 64;
		}
		for ( ; inlen && hd->count < 64; inlen--)
			hd->buf[hd->count++] = *inbuf++;
	}
	
	/* The routine terminates the computation */
	void tiger_final(unsigned char *hash, cc_tiger_ctx *hd)
	{
		uint32_t t, msb, lsb;
		unsigned char *p;
		int i, j;
	
		tiger_update(hd, NULL, 0); /* flush */;
	
		msb = 0;
		t = hd->nblocks;
		if ((lsb = t << 6) < t)  /* multiply by 64 to make a byte count */
			msb++;
	
		msb += t >> 26;
		t = lsb;
		if ((lsb = t + hd->count) < t)  /* add the count */
			msb++;
	
		t = lsb;
		if ((lsb = t << 3) < t)  /* multiply by 8 to make a bit count */
			msb++;
	
		msb += t >> 29;
	
		if (hd->count < 56) {  /* enough room */
			hd->buf[hd->count++] = 0x01; /* pad */
			while (hd->count < 56)
				hd->buf[hd->count++] = 0; /* pad */
		} else {  /* need one extra block */
			hd->buf[hd->count++] = 0x01; /* pad character */
			while (hd->count < 64)
				hd->buf[hd->count++] = 0;
			tiger_update(hd, NULL, 0); /* flush */;
			memset(hd->buf, 0, 56);    /* fill next block with zeroes */
		}
	
		/* append the 64 bit count */
		hd->buf[56] = lsb      ;
		hd->buf[57] = lsb >> 8;
		hd->buf[58] = lsb >> 16;
		hd->buf[59] = lsb >> 24;
		hd->buf[60] = msb      ;
		hd->buf[61] = msb >> 8;
		hd->buf[62] = msb >> 16;
		hd->buf[63] = msb >> 24;
		transform(hd, hd->buf);
	
		p = hd->buf;
	  #ifdef TARGET_BIG_ENDIAN
	#define X(a) do { *(uint64_t *)p = hd->a ; p += 8; } while (0)
		/* Original code - modified by jk to deal with gcc changes */
		/*    #define X(a) do { *(u64*)p = hd->##a ; p += 8; } while(0) */
	  #else   /* little endian */
	#define X(a) do { *p++ = (unsigned char)(hd->a >> 56); *p++ = (unsigned char)(hd->a >> 48); \
					  *p++ = (unsigned char)(hd->a >> 40); *p++ = (unsigned char)(hd->a >> 32); \
					  *p++ = (unsigned char)(hd->a >> 24); *p++ = (unsigned char)(hd->a >> 16); \
					  *p++ = (unsigned char)(hd->a >> 8); *p++ = (unsigned char)(hd->a); } while (0)
	
		/* Original code - modified by jk to deal with gcc changes
		 #define X(a) do { *p++ = hd->##a >> 56; *p++ = hd->##a >> 48; \
		 *p++ = hd->##a >> 40; *p++ = hd->##a >> 32; \
		 *p++ = hd->##a >> 24; *p++ = hd->##a >> 16; \
		 *p++ = hd->##a >>  8; *p++ = hd->##a; } while(0)
		 */
	
	  #endif
		X(a);
		X(b);
		X(c);
	  #undef X
	
		/* unpack the hash */
		/* Modified by jk to produce little endian output like MD5 or SHA-1 */
		j = 0;
		for (i = 0; i < 8; i++)
			hash[j++] = (unsigned char)((hd->a >> 8 * i) & 0xff);
	
		j = 8;
		for (i = 0; i < 8; i++)
			hash[j++] = (unsigned char)((hd->b >> 8 * i) & 0xff);
	
		j = 16;
		for (i = 0; i < 8; i++)
			hash[j++] = (unsigned char)((hd->c >> 8 * i) & 0xff);
	
		/* Original code, produces output in big endian
		 * j=7;
		 * for (i=0; i<8; i++)
		 * hash[j--] = (hd->a >> 8*i) & 0xff;
		 * j=15;
		 * for (i=0; i<8; i++)
		 * hash[j--] = (hd->b >> 8*i) & 0xff;
		 * j=23;
		 * for (i=0; i<8; i++)
		 * hash[j--] = (hd->c >> 8*i) & 0xff;
		 */
	}
	
}

namespace CrissCross
{
	namespace Crypto
	{
		TigerHash::TigerHash() : m_hashString(NULL), m_hash(NULL)
		{
			Reset();
		}

		TigerHash::~TigerHash()
		{
			Reset();
		}

		int TigerHash::Process(const void * _data, size_t _length)
		{
			CoreAssert(this != NULL);

			Reset();
			if (!_data) return -1;

			tiger_update(&m_state, (unsigned char *)_data, _length);
			m_hash = new unsigned char[TIGER_DIGEST_SIZE];
			tiger_final(m_hash, &m_state);
			return 0;
		}

		int TigerHash::Process(CrissCross::IO::CoreIOReader *_reader)
		{
			CoreAssert(this != NULL);

			Reset();
			if (!_reader) return -1;

			int64_t pos = _reader->Position();
			_reader->Seek(0);
			char buffer[8192];
			size_t bytesRead = 0;
			do
			{
				bytesRead = _reader->ReadBlock(buffer, sizeof(buffer));
				if (bytesRead > 0)
					ProcessBlock(buffer, bytesRead);
			} while (bytesRead == sizeof(buffer) && !_reader->EndOfFile());
			Finalize();
			_reader->Seek(pos);
			return 0;
		}

		int TigerHash::ProcessBlock(const void * _data, size_t _length)
		{
			CoreAssert(this != NULL);

			if (!_data) return -1;

			tiger_update(&m_state, (unsigned char *)_data, _length);
			return 0;
		}

		void TigerHash::Finalize()
		{
			CoreAssert(this != NULL);

			if (m_hash) delete [] m_hash;

			m_hash = new unsigned char[TIGER_DIGEST_SIZE];
			tiger_final(m_hash, &m_state);
		}

		const char *TigerHash::ToString() const
		{
			CoreAssert(this != NULL);

			if (m_hashString) return m_hashString;

			m_hashString = new char[TIGER_DIGEST_SIZE * 2 + 1];
			for (int i = 0; i < TIGER_DIGEST_SIZE; i++)
				sprintf(m_hashString + (i * 2), "%02x", m_hash[i]);

			return m_hashString;
		}

		void TigerHash::Reset()
		{
			CoreAssert(this != NULL);

			delete [] m_hash; m_hash = NULL;
			delete [] m_hashString; m_hashString = NULL;

			tiger_init(&m_state);
		}

		bool TigerHash::operator==(const TigerHash &_other) const
		{
			CoreAssert(this != NULL);

			return (memcmp(m_hash, _other.m_hash, TIGER_DIGEST_SIZE) == 0);
		}
	}
}

