/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:56:32 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 14:00:04 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
 * Binary min-heap (priority queue) stored in an array: for index i the
 * parent is (i - 1) / 2, the children are 2i + 1 and 2i + 2.
 * Heap property: every parent is "better" than its children -> the best
 * request is always items[0]. Every dongle has its own heap; with at most
 * 2 neighbours it holds max. 2 requests, but the code works for any size.
 */

/*
 * heap_cmp - is items[a] better (= more urgent) than items[b]?
 * FIFO: smaller seq (earlier arrival) wins.
 * EDF: earlier deadline wins, equal deadlines fall back to seq.
 * Returns 1 if a is better, 0 otherwise (also 0 when equal).
 */
static	int	heap_cmp(t_heap *heap, int scheduler, int a, int b)
{
	if (scheduler == FIFO)
		return (heap->items[a].seq < heap->items[b].seq);
	else
	{
		if (heap->items[a].deadline != heap->items[b].deadline)
			return (heap->items[a].deadline < heap->items[b].deadline);
		return (heap->items[a].seq < heap->items[b].seq);
	}
}

/*
 * heap_swap - swaps the requests at index a and b (struct copy).
 */
static	void	heap_swap(t_heap *heap, int a, int b)
{
	t_request	temp;

	temp = heap->items[a];
	heap->items[a] = heap->items[b];
	heap->items[b] = temp;
}

/*
 * heap_sift_down - lets the element at index a sink until the heap
 * property holds again: swap with the better child as long as that child
 * is better than the element. Used after pop.
 */
static	void	heap_sift_down(t_heap *heap, int a, int scheduler)
{
	int	i;
	int	best;
	int	left;

	i = a;
	while ((2 * i) + 1 < heap->size)
	{
		best = i;
		left = (2 * i) + 1;
		if (heap_cmp(heap, scheduler, left, i))
			best = (2 * i) + 1;
		if ((2 * i) + 2 < heap->size
			&& heap_cmp(heap, scheduler, (2 * i) + 2, best))
			best = (2 * i) + 2;
		if (best == i)
			return ;
		heap_swap(heap, i, best);
		i = best;
	}
}

/*
 * heap_push - inserts a copy of request and restores the heap property
 * (sift up: swap with the parent while the new element is better).
 * Caller must hold the mutex of the dongle that owns the heap.
 * Returns SUCESS, or FAIL if the heap is full.
 */
int	heap_push(t_heap *heap, int scheduler, t_request request)
{
	int	i;

	if (heap->size >= HEAP_CAP)
		return (FAIL);
	heap->items[heap->size] = request;
	heap->size++;
	i = heap->size - 1;
	while (i > 0 && heap_cmp(heap, scheduler, i, (i - 1) / 2))
	{
		heap_swap(heap, i, (i - 1) / 2);
		i = (i - 1) / 2;
	}
	return (SUCESS);
}

/*
 * heap_pop - removes the best request (items[0]): the last element moves
 * to the root and sinks down. The removed request is not returned, the
 * caller already knows it was itself (checked via items[0].coder).
 * Caller must hold the mutex of the dongle that owns the heap.
 * Returns SUCESS, or FAIL if the heap is empty.
 */
int	heap_pop(t_heap *heap, int scheduler)
{
	if (heap->size == 0)
		return (FAIL);
	heap->size--;
	heap->items[0] = heap->items[heap->size];
	heap_sift_down(heap, 0, scheduler);
	return (SUCESS);
}
