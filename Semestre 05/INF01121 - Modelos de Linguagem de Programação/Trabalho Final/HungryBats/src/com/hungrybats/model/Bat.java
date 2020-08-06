package com.hungrybats.model;

import android.graphics.Bitmap;

public class Bat extends Model {
	
	private boolean touched; //Indica se o morcego foi ou n�o selecionado

	/* Construtor da classe */
	public Bat(Bitmap bitmap, int x, int y) {
		super(bitmap, x, y);
	}
	
	/* Setter e Getter para a vari�vel touched */
	public boolean isTouched() {
		return touched;
	}
	public void setTouched(boolean touched) {
		this.touched = touched;
	}

	/*
	 * Trabalha com o evento de MotionEvent.ACTION_DOWN, ou seja, o usu�rio 
	 * clicou com o dedo na tela. Se o evento ocorre na superf�cie de bitmap 
	 * do morcego, ent�o o estado de touched deve passar para true, sen�o
	 * para false. 
	 * 
	 * @param eventX - a coordenada x do evento
	 * @param eventY - a coordenada y do evento
	 */
	public void handleActionDown(int eventX, int eventY) {
		if(eventX >= (this.x - this.bitmap.getWidth() / 2) && (eventX <= (this.x + this.bitmap.getWidth()/2))) {
			if(eventY >= (this.y - this.bitmap.getHeight() / 2) && (this.y <= (this.y + this.bitmap.getHeight() / 2))) {
				this.setTouched(true); //morcego foi tocado
			} else {
				this.setTouched(false); //morcego n�o foi tocado
			}
		} else {
			this.setTouched(false); 
		}
	}
	
	
}
