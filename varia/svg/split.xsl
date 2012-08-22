<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	xmlns:lxslt="http://xml.apache.org/xslt"
	xmlns:redirect="http://xml.apache.org/xalan/redirect"
	xmlns:stringutils="xalan://org.apache.tools.ant.util.StringUtils"
	extension-element-prefixes="redirect"
	xmlns:svg="http://www.w3.org/2000/svg">
	<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>
	
	<!--
		
		Stylesheet that extract the glyhps of a font
		Each glyph is written in a separate file
		A 'scale(1.0, -1.0)' transform attribute is added to flip the glyph
		
	-->
	
	<!-- this is the out directory -->
	<xsl:param name="output.dir" select="'../../data/svg'"/>

	<!-- this is the out directory for the bounding  box svg -->
	<xsl:param name="output.bb.dir" select="'.'"/>
	

	<xsl:template match="/">	
		
		<redirect:write file="{$output.bb.dir}/boundingbox.svg">
			<xsl:text disable-output-escaping="yes"><![CDATA[<?xml version="1.0" encoding="utf-8"?>
<svg viewBox="-60 2.0 699.1 189.5" version="1.1" xmlns="http://www.w3.org/2000/svg"  xmlns:xlink="http://www.w3.org/1999/xlink">]]></xsl:text>
		</redirect:write>
		
		<xsl:apply-templates select="*"/>
		
		<redirect:write file="{$output.bb.dir}/boundingbox.svg" append="true">
			<xsl:text disable-output-escaping="yes"><![CDATA[	  <script xlink:href="showbboxdims.js"/>
				</svg>]]></xsl:text>
		</redirect:write>
		
		
	</xsl:template>
	
	<xsl:template match="svg:glyph">
		<xsl:variable name="glyph">
			<xsl:choose>
				<!-- figures -->
				<xsl:when test="@unicode='0'"><xsl:value-of select="'figure_0'"/></xsl:when>
				<xsl:when test="@unicode='1'"><xsl:value-of select="'figure_1'"/></xsl:when>
				<xsl:when test="@unicode='2'"><xsl:value-of select="'figure_2'"/></xsl:when>
				<xsl:when test="@unicode='3'"><xsl:value-of select="'figure_3'"/></xsl:when>
				<xsl:when test="@unicode='4'"><xsl:value-of select="'figure_4'"/></xsl:when>
				<xsl:when test="@unicode='5'"><xsl:value-of select="'figure_5'"/></xsl:when>
				<xsl:when test="@unicode='6'"><xsl:value-of select="'figure_6'"/></xsl:when>
				<xsl:when test="@unicode='7'"><xsl:value-of select="'figure_7'"/></xsl:when>
				<xsl:when test="@unicode='8'"><xsl:value-of select="'figure_8'"/></xsl:when>
				<xsl:when test="@unicode='9'"><xsl:value-of select="'figure_9'"/></xsl:when>
				<!-- meter signs -->
				<xsl:when test="@unicode='&#x7f;'"><xsl:value-of select="'meter_symb_2_cut'"/></xsl:when>
				<xsl:when test="@unicode='&#xf9;'"><xsl:value-of select="'meter_symb_3_cut'"/></xsl:when>
				<xsl:when test="@unicode='&#xb7;'"><xsl:value-of select="'meter_symb_cut'"/></xsl:when>
				<xsl:when test="@unicode='&#x152;'"><xsl:value-of select="'meter_symb_common'"/></xsl:when>
				<!-- oblique figures -->
				<xsl:when test="@unicode='&#x201a;'"><xsl:value-of select="'oblique_figure_0'"/></xsl:when>
				<xsl:when test="@unicode='&#x192;'"><xsl:value-of select="'oblique_figure_1'"/></xsl:when>
				<xsl:when test="@unicode='&#x201e;'"><xsl:value-of select="'oblique_figure_2'"/></xsl:when>
				<xsl:when test="@unicode='&#x2026;'"><xsl:value-of select="'oblique_figure_3'"/></xsl:when>
				<xsl:when test="@unicode='&#x2020;'"><xsl:value-of select="'oblique_figure_4'"/></xsl:when>
				<xsl:when test="@unicode='&#x2021;'"><xsl:value-of select="'oblique_figure_5'"/></xsl:when>
				<xsl:when test="@unicode='&#x2c6;'"><xsl:value-of select="'oblique_figure_6'"/></xsl:when>
				<xsl:when test="@unicode='&#x2030;'"><xsl:value-of select="'oblique_figure_7'"/></xsl:when>
				<xsl:when test="@unicode='&#x160;'"><xsl:value-of select="'oblique_figure_8'"/></xsl:when>
				<xsl:when test="@unicode='&#x2039;'"><xsl:value-of select="'oblique_figure_9'"/></xsl:when>
				<!-- clef -->
				<xsl:when test="@unicode='&#xcf;'"><xsl:value-of select="'clef_G'"/></xsl:when>
				<xsl:when test="@unicode='&#xd0;'"><xsl:value-of select="'clef_F'"/></xsl:when>
				<xsl:when test="@unicode='&#xd1;'"><xsl:value-of select="'clef_C'"/></xsl:when>
				<xsl:when test="@unicode='&#xd2;'"><xsl:value-of select="'clef_G8'"/></xsl:when>
				<xsl:when test="@unicode='&#xe3;'"><xsl:value-of select="'clef_G_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe4;'"><xsl:value-of select="'clef_F_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe5;'"><xsl:value-of select="'clef_C_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe6;'"><xsl:value-of select="'clef_G_chiavette'"/></xsl:when>
				<!-- alterations -->
				<xsl:when test="@unicode='&#xd3;'"><xsl:value-of select="'alt_sharp'"/></xsl:when>
				<xsl:when test="@unicode='&#xd4;'"><xsl:value-of select="'alt_natural'"/></xsl:when>
				<xsl:when test="@unicode='&#xd5;'"><xsl:value-of select="'alt_flat'"/></xsl:when>
				<xsl:when test="@unicode='&#xd6;'"><xsl:value-of select="'alt_double_sharp'"/></xsl:when>
				<xsl:when test="@unicode='&#xe7;'"><xsl:value-of select="'alt_sharp_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe8;'"><xsl:value-of select="'alt_natural_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe9;'"><xsl:value-of select="'alt_flat_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xea;'"><xsl:value-of select="'alt_double_sharp_mensural'"/></xsl:when>
				<!-- rests -->
				<xsl:when test="@unicode='&#xd7;'"><xsl:value-of select="'rest_4'"/></xsl:when>
				<xsl:when test="@unicode='&#xd8;'"><xsl:value-of select="'rest_8'"/></xsl:when>
				<xsl:when test="@unicode='&#xd9;'"><xsl:value-of select="'rest_16'"/></xsl:when>
				<xsl:when test="@unicode='&#xda;'"><xsl:value-of select="'rest_32'"/></xsl:when>
				<xsl:when test="@unicode='&#xdb;'"><xsl:value-of select="'rest_64'"/></xsl:when>
				<xsl:when test="@unicode='&#xdc;'"><xsl:value-of select="'rest_128'"/></xsl:when>
				<xsl:when test="@unicode='&#xeb;'"><xsl:value-of select="'rest_4_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xec;'"><xsl:value-of select="'rest_8_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xed;'"><xsl:value-of select="'rest_16_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xee;'"><xsl:value-of select="'rest_32_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xef;'"><xsl:value-of select="'rest_64_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xf0;'"><xsl:value-of select="'rest_128_mensural'"/></xsl:when>
				<!-- note heads -->
				<xsl:when test="@unicode='&#xc9;'"><xsl:value-of select="'head_whole'"/></xsl:when>
				<xsl:when test="@unicode='&#xca;'"><xsl:value-of select="'head_whole_fill'"/></xsl:when>
				<xsl:when test="@unicode='&#xcb;'"><xsl:value-of select="'head_half'"/></xsl:when>
				<xsl:when test="@unicode='&#xcc;'"><xsl:value-of select="'head_quarter'"/></xsl:when>
				<xsl:when test="@unicode='&#xdd;'"><xsl:value-of select="'head_whole_diamond'"/></xsl:when>
				<xsl:when test="@unicode='&#xde;'"><xsl:value-of select="'head_whole_filldiamond'"/></xsl:when>
				<xsl:when test="@unicode='&#xdf;'"><xsl:value-of select="'head_half_diamond'"/></xsl:when>
				<xsl:when test="@unicode='&#xe0;'"><xsl:value-of select="'head_quarter_filldiamond'"/></xsl:when>
				<!-- slashes -->
				<xsl:when test="@unicode='&#xcd;'"><xsl:value-of select="'slash_up'"/></xsl:when>
				<xsl:when test="@unicode='&#xce;'"><xsl:value-of select="'slash_down'"/></xsl:when>
				<xsl:when test="@unicode='&#xe1;'"><xsl:value-of select="'slash_up_mensural'"/></xsl:when>
				<xsl:when test="@unicode='&#xe2;'"><xsl:value-of select="'slash_down_mensural'"/></xsl:when>
				<!-- ornaments -->
				<xsl:when test="@unicode='&#x23;'"><xsl:value-of select="'orn_mordent'"/></xsl:when>
				<!-- todo -->
				<xsl:otherwise><xsl:value-of select="'unknown'"/></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:if test="$glyph != 'unknown'">
			<redirect:write file="{$output.dir}/{$glyph}.xml">
				<xsl:element name="path">
					<xsl:attribute name="id">
						<xsl:value-of select="$glyph"/>
					</xsl:attribute>
					<xsl:attribute name="d">
						<xsl:value-of select="@d"/>
					</xsl:attribute>
					<!-- we need to flip the glyph because coordinate sytem 
						in svg fonts is upside-donw --> 
					<xsl:attribute name="transform">
						<xsl:text>scale(1.0, -1.0)</xsl:text>
					</xsl:attribute>
				</xsl:element>
			</redirect:write>

			<redirect:write file="{$output.bb.dir}/boundingbox.svg" append="true">
				<xsl:element name="path">
					<xsl:attribute name="id">
						<xsl:value-of select="$glyph"/>
					</xsl:attribute>
					<xsl:attribute name="d">
						<xsl:value-of select="@d"/>
					</xsl:attribute>
					<!-- we need to flip the glyph because coordinate sytem 
						in svg fonts is upside-donw --> 
					<xsl:attribute name="transform">
						<xsl:text>scale(1.0, -1.0)</xsl:text>
					</xsl:attribute>
				</xsl:element>				
		</redirect:write>
		</xsl:if>
	</xsl:template>
	
</xsl:stylesheet>